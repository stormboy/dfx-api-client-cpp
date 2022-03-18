// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/websocket/MeasurementStreamWebSocket.hpp"
#include "dfx/api/CloudLog.hpp"
#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/websocket/CloudWebSocket.hpp"

#include "dfx/proto/measurements.pb.h"

#include "fmt/format.h"
#include <chrono>
#include <google/protobuf/util/json_util.h>
#include <thread>

using dfx::api::CloudAPI;
using dfx::api::CloudConfig;
using dfx::api::CloudStatus;
using dfx::api::MeasurementStreamAPI;

using namespace dfx::api;
using namespace dfx::api::websocket;

using namespace std::chrono_literals;

MeasurementStreamWebSocket::MeasurementStreamWebSocket(const CloudConfig& config,
                                                       const std::shared_ptr<CloudWebSocket>& cloudWebSocket)
    : cloudWebSocket(std::move(cloudWebSocket))
{
    initialize();
}

MeasurementStreamWebSocket::~MeasurementStreamWebSocket()
{
    closeStream();
}

void MeasurementStreamWebSocket::initialize()
{
    streamOpen = false;
    measurementID = "";
    chunkOrder = 0;
    isFirstChunk = true;
    writerClosedStream = false;
    lastChunkSent = false;
    chunksOutstanding = 0;
}

CloudStatus MeasurementStreamWebSocket::setupStream(const CloudConfig& config, const std::string& studyID)
{
    DFX_CLOUD_VALIDATOR_MACRO(MeasurementStreamValidator, setupStream(config, studyID));

    std::unique_lock<std::recursive_mutex> lock(mutex);

    if (streamOpen) {
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR,
                           fmt::format("stream already created, must call reset before reuse"));
    }

    if (studyID.empty()) {
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, fmt::format("{} is empty", "studyID"));
    }
    if (config.deviceToken.empty()) {
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, fmt::format("{} is empty", "config.deviceToken"));
    }
    if (config.authToken.empty()) {
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, fmt::format("{} is empty", "config.userToken"));
    }

    // Create Measurement --> MeasurementID
    {
        dfx::proto::measurements::CreateRequest request;
        dfx::proto::measurements::CreateResponse response;

        request.set_studyid(studyID);
        auto status = cloudWebSocket->sendMessage(dfx::api::web::Measurements::Create, request, response);
        if (!status.OK()) {
            return status;
        }

        measurementID = response.id();
    }

    {
        // STRM is nothing special but nice to look at when debugging, API just wants
        // a random 10 byte transaction ID
        requestID = fmt::format("STRM{:06}", 1010);

        dfx::proto::measurements::SubscribeResultsRequest request;
        dfx::proto::measurements::SubscribeResultsResponse response;

        request.mutable_params()->set_id(measurementID);
        request.set_requestid(requestID);

        cloudWebSocket->registerStream(requestID, this);
        auto status = cloudWebSocket->sendMessage(dfx::api::web::Measurements::SubscribeResults, request, response);
        if (!status.OK()) {
            closeStream();
            return status;
        }
    }

    streamOpen = true;
    return CloudStatus(CLOUD_OK);
}

CloudStatus MeasurementStreamWebSocket::closeStream()
{
    // NOTE: This method is used by the serviceThread to close the measurement.
    // It MUST NOT explicitly call webSocket->close() which would dead lock join()
    // attempting to merge with itself.
    //    cloudWebSocket->webSocket->close();
    return closeMeasurement(CloudStatus(CLOUD_OK));
}

CloudStatus MeasurementStreamWebSocket::sendChunk(const CloudConfig& config,
                                                  const std::vector<uint8_t>& chunk,
                                                  bool isLastChunk)
{
    CloudStatus status(CLOUD_OK);

    // Need to check before waiting on condition, reader thread may never set
    if (isMeasurementClosed(status)) {
        return status; // if it has already been closed.
    }

    dfx::proto::measurements::DataRequest request;
    dfx::proto::measurements::DataResponse response;

    request.mutable_params()->set_id(measurementID);
    if (!isLastChunk) {
        if (chunkOrder == 0) {
            request.set_action("FIRST::PROCESS");
        } else {
            request.set_action("CHUNK::PROCESS");
        }
    } else {
        request.set_action("LAST::PROCESS");
        lastChunkSent = true;
    }

    const std::string payload(chunk.begin(), chunk.end());
    request.set_payload(payload);

    status = cloudWebSocket->sendMessage(dfx::api::web::Measurements::Data, request, response);
    if (!status.OK()) {
        cloudLog(CLOUD_LOG_LEVEL_WARNING, "WEB: Send not okay %d: %s", status.code, status.message.c_str());

        // Our write failed, connection bad close stream and return status
        return closeStream();
    }

    chunksOutstanding++;

    return CloudStatus(CLOUD_OK);
}

CloudStatus MeasurementStreamWebSocket::cancel(const CloudConfig& config)
{
    CloudStatus status(CLOUD_OK);
    std::unique_lock<std::recursive_mutex> lock(mutex);

    // If measurement has already been closed, we don't want to attempt a cancel on a bad stream
    if (isMeasurementClosed(status)) {
        return status;
    }

    return status;
}

void MeasurementStreamWebSocket::handleStreamResponse(const std::shared_ptr<std::vector<uint8_t>>& message)
{
    // This is a stream response, need to decode enough of it to decide what type
    // of stream response we are being sent.
    auto rawData = reinterpret_cast<const char*>(message->data());

    auto rawSize = message->size();
    assert(rawSize < INT_MAX); // Explicit cast - something wrong if this big

    int messageSize = static_cast<int>(rawSize);

    std::string statusCode(rawData + 10, 3);

    // Request is considered OK
    if (statusCode != "200") {
        auto status = cloudWebSocket->decodeWebSocketError(
            statusCode, std::vector<uint8_t>(rawData + 13, rawData + messageSize - 13));
        cloudLog(CLOUD_LOG_LEVEL_WARNING, "WEB: Response status bad %d: %s", status.code, status.message.c_str());
        closeMeasurement(status);
    } else {
        dfx::proto::measurements::SubscribeResultsResponse response;

        if (!response.ParseFromArray(rawData + 13, messageSize - 13)) {
            cloudLog(CLOUD_LOG_LEVEL_WARNING, "WEB: Response decode failed");
            closeMeasurement(CloudStatus(CLOUD_INTERNAL_ERROR, "WEB: Response decode failed"));
        } else {
            if (response.has_error()) {
                const auto& error = response.error();
                const auto& errorCode = error.code();
                if (errorCode != "OK") {
                    MeasurementWarning warning;
                    warning.warningCode = -1;                     // Code & message, both strings :(
                    warning.warningMessage = error.DebugString(); // Hopefully it says something
                    warning.timestampMS = 0;                      // Nothing available
                    handleWarning(warning);
                }
            }

            if (response.measurementid().length() > 0) {
                if (measurementID.empty()) { // Only send client measurement ID once per measurement
                    handleMeasurementID(response.measurementid());
                    measurementID = response.measurementid();
                }
            }

            auto multiplier = static_cast<float>(response.multiplier());
            if (multiplier == 0) {
                multiplier = 1; // Extra cautious to avoid divide by zero below
            }

            int chunkNumber(0);
            auto measurementDataID = response.measurementdataid();
            if (measurementDataID.length() > measurementID.length() + 1) { // Expect measurementID:#
                measurementDataID = measurementDataID.substr(measurementID.length() + 1);
                chunkNumber = std::stoi(measurementDataID);
            }

            MeasurementResult result;
            result.faceID = "1"; // V2 WebSocket only supports one face ID presently
            result.chunkOrder = chunkNumber;

            // /////////////////////////////////////////
            // SubscribeResultsResponse Proto Definition
            // /////////////////////////////////////////
            //
            // message SubscribeResultsResponse {
            //  message Channel {
            //        repeated sint32 Data = 1 [packed=true];
            //        string Channel = 2;
            //  }
            //  string MeasurementID = 1 ;
            //  string MeasurementDataID = 2 ;
            //  string MeasurementResultID = 3 ;
            //  uint32 Multiplier = 4;
            //  map<string, Channel> Channels = 5;
            //  Error Error = 6;
            //}
            for (auto& channel : response.channels()) {
                std::string signalName = channel.first;
                std::string channelName = channel.second.channel();

                std::vector<float> data;
                auto& measurementData = channel.second.data();
                data.reserve(measurementData.size());
                for (const auto& value : measurementData) {
                    // Data in measurementData is type float, the v2 interface was
                    // designed to hand back doubles.
                    data.push_back(static_cast<float>(value) / multiplier);
                }

                result.signalData[signalName] = data;
            }

            if (result.signalData.size() > 0) {
                handleResult(result);
            }

            chunksOutstanding--;
            if (lastChunkSent && chunksOutstanding == 0) {
                closeStream(); // All responses received, shut the stream down
            }
        }
    }
}
