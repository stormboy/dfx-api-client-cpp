// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/CloudLog.hpp"
#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/websocket/json/CloudWebSocketJson.hpp"
#include "dfx/api/websocket/json/MeasurementStreamWebSocketJson.hpp"

#include "fmt/format.h"
#include "libbase64.h"
#include <chrono>
#include <thread>

using dfx::api::CloudAPI;
using dfx::api::CloudConfig;
using dfx::api::CloudStatus;
using dfx::api::MeasurementStreamAPI;

using namespace dfx::api;
using namespace dfx::api::websocket::json;

using namespace std::chrono_literals;

MeasurementStreamWebSocketJson::MeasurementStreamWebSocketJson(
    const CloudConfig& config, const std::shared_ptr<CloudWebSocketJson>& cloudWebSocketJson)
    : cloudWebSocketJson(std::move(cloudWebSocketJson))
{
    initialize();
}

MeasurementStreamWebSocketJson::~MeasurementStreamWebSocketJson()
{
    closeStream();
}

void MeasurementStreamWebSocketJson::initialize()
{
    streamOpen = false;
    measurementID = "";
    chunkOrder = 0;
    isFirstChunk = true;
    writerClosedStream = false;
    lastChunkSent = false;
    chunksOutstanding = 0;
}

CloudStatus MeasurementStreamWebSocketJson::setupStream(const CloudConfig& config,
                                                        const std::string& studyID,
                                                        const std::map<CreateProperty, std::string>& properties)
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

    // Create Measurement --> MeasurementID
    {
        nlohmann::json request;

        if (properties.size() > 0) {
            {
                auto it = properties.find(CreateProperty::UserProfileID);
                if (it != properties.end()) {
                    request["UserProfileID"] = it->second;
                }
            }
            {
                auto it = properties.find(CreateProperty::PartnerID);
                if (it != properties.end()) {
                    request["PartnerID"] = it->second;
                }
            }
            {
                auto it = properties.find(CreateProperty::Resolution);
                if (it != properties.end()) {
                    int resolution = 0;
                    try {
                        resolution = std::stoi(it->second);
                    } catch (...) {
                        // Ignore
                    }
                    if (resolution > 0) {
                        resolution = 100;
                    }
                    request["Resolution"] = resolution;
                }
            }
        }
        request["StudyID"] = studyID;

        nlohmann::json response;

        // https://dfxapiversion10.docs.apiary.io/#reference/0/measurements/create
        auto result = cloudWebSocketJson->sendMessageJson(config, web::Measurements::Create, {}, {}, request, response);
        if (!result.OK()) {
            return result;
        }

        measurementID = response["ID"];
    }

    {
        // STRM is nothing special but nice to look at when debugging, API just wants
        // a random 10 byte transaction ID
        requestID = fmt::format("STRM{:06}", 1010);

        nlohmann::json request;
        request["RequestID"] = requestID;

        nlohmann::json response;

        nlohmann::json params;
        params["ID"] = measurementID;

        cloudWebSocketJson->registerStream(requestID, this);

        // https://dfxapiversion10.docs.apiary.io/#reference/0/measurements/subscribe-to-results
        auto result = cloudWebSocketJson->sendMessageJson(
            config, web::Measurements::SubscribeResults, params, {}, request, response);

        if (!result.OK()) {
            closeStream();
            return result;
        }
    }

    streamOpen = true;
    return CloudStatus(CLOUD_OK);
}

CloudStatus MeasurementStreamWebSocketJson::closeStream()
{
    // NOTE: This method is used by the serviceThread to close the measurement.
    // It MUST NOT explicitly call webSocket->close() which would dead lock join()
    // attempting to merge with itself.
    //    cloudWebSocket->webSocket->close();
    return closeMeasurement(CloudStatus(CLOUD_OK));
}

CloudStatus MeasurementStreamWebSocketJson::sendChunk(const CloudConfig& config,
                                                      const std::vector<uint8_t>& chunk,
                                                      bool isLastChunk)
{
    CloudStatus result(CLOUD_OK);

    // Need to check before waiting on condition, reader thread may never set
    if (isMeasurementClosed(result)) {
        return result; // if it has already been closed.
    }

    nlohmann::json request;

    if (!isLastChunk) {
        if (isFirstChunk) {
            request["Action"] = "FIRST::PROCESS";
            isFirstChunk = false;
        } else {
            request["Action"] = "CHUNK::PROCESS";
        }
    } else {
        request["Action"] = "LAST::PROCESS";
        lastChunkSent = true;
    }

    // Base64 encode the chunk
    const char* unencodedData = reinterpret_cast<const char*>(chunk.data());
    size_t unencodedLength = chunk.size();
    std::vector<char> chunkBase64;
    chunkBase64.resize(chunk.size()*2);
    size_t encoded_length = 0;
    int flags = 0;
    base64_encode(unencodedData, unencodedLength, chunkBase64.data(), &encoded_length, flags );
    chunkBase64.resize(encoded_length);
    const std::string payload(chunkBase64.begin(), chunkBase64.end());
    request["Payload"] = payload;

    nlohmann::json response;

    nlohmann::json params;
    params["ID"] = measurementID;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/measurements/add-data
    result = cloudWebSocketJson->sendMessageJson(config, web::Measurements::Data, params, {}, request, response);

    if (!result.OK()) {
        cloudLog(CLOUD_LOG_LEVEL_WARNING, "WEB: Send not okay %d: %s", result.code, result.message.c_str());

        // Our write failed, connection bad close stream and return status
        closeStream();
        return result;
    }

    {
        const std::lock_guard<std::mutex> lock(mutexChunks);
        if (isLastChunk) {
            lastChunkSent = true;
        }
        chunksOutstanding++;
    }

    return CloudStatus(CLOUD_OK);
}

CloudStatus MeasurementStreamWebSocketJson::reset(const CloudConfig& config) 
{
    CloudStatus status(CLOUD_OK);
    isFirstChunk = true;

    return status;
}

CloudStatus MeasurementStreamWebSocketJson::cancel(const CloudConfig& config)
{
    CloudStatus status(CLOUD_OK);
    std::unique_lock<std::recursive_mutex> lock(mutex);

    // If measurement has already been closed, we don't want to attempt a cancel on a bad stream
    if (isMeasurementClosed(status)) {
        return status;
    }

    return status;
}

void MeasurementStreamWebSocketJson::handleStreamResponse(const std::shared_ptr<std::vector<uint8_t>>& message)
{
    // This is a stream response, need to decode enough of it to decide what type
    // of stream response we are being sent.
    auto rawData = reinterpret_cast<const char*>(message->data());

    auto rawSize = message->size();
    assert(rawSize < INT_MAX); // Explicit cast - something wrong if this big

    int messageSize = static_cast<int>(rawSize);

    std::string statusCode(rawData + 10, 3);

    auto responseString = std::string(rawData + 13, messageSize - 13);

    bool allow_exceptions = false;
    nlohmann::json response = nlohmann::json::parse(responseString, nullptr, allow_exceptions);

    // Request is considered OK
    if (statusCode != "200") {
        auto result = CloudStatus(CLOUD_INTERNAL_ERROR);
        if (!response.is_discarded() && response.contains("Code")) {
            std::string code = response["Code"];
            std::string errors;
            if (response.contains("Errors")) {
                errors = response["Errors"].dump();
            }
            cloudLog(CLOUD_LOG_LEVEL_WARNING, "WEB: Response status bad %s: %s", code.c_str(), errors.c_str());
        } else {
            cloudLog(CLOUD_LOG_LEVEL_WARNING, "WEB: Response status bad %d: %s", result.code, result.message.c_str());
        }
        closeMeasurement(result);
    } else {
        if (response.is_discarded()) {
            cloudLog(CLOUD_LOG_LEVEL_WARNING, "WEB: Response decode failed");
            closeMeasurement(CloudStatus(CLOUD_INTERNAL_ERROR, "WEB: Response decode failed"));
        } else {
            if (response.contains("Error")) {
                const auto& error = response["Error"];
                const auto& errorCode = error["Code"].get<std::string>();
                if (errorCode != "OK") {
                    for (const auto& error : error["Errors"].items()) {
                        MeasurementWarning warning{};
                        warning.warningCode = -1;               // Code & message, both strings :(

                        // Hopefully it says something useful
                        warning.warningMessage = error.key() + ": " + error.value()["msgs"].dump();
                        warning.timestampMS = 0;                // Nothing available
                        handleWarning(warning);
                    }
                }
            }

            if (response.contains("ID")) {
                if (measurementID.empty()) { // Only send client measurement ID once per measurement
                    auto id = response["ID"].get<std::string>();
                    handleMeasurementID(id);
                    measurementID = id;
                }
            }

            auto multiplier = 1;
            if (response.contains("Multiplier")) {
                multiplier = response["Multiplier"].get<int>();
            }

            int chunkNumber(0);
            auto measurementDataID = response["MeasurementDataID"].get<std::string>();
            if (measurementDataID.length() > measurementID.length() + 1) { // Expect measurementID:#
                measurementDataID = measurementDataID.substr(measurementID.length() + 1);
                chunkNumber = std::stoi(measurementDataID);
            }

            MeasurementResult result{};
            result.faceID = "1"; // V2 WebSocket only supports one face ID presently
            result.chunkOrder = chunkNumber;
            result.timestampMS = 0; // Nothing available
            result.frameEndTimestampMS = 0;

            for (const auto& entry : response["Channels"].items()) {
                std::string signalName = entry.key();
                const auto& signalValue = entry.value();
                std::string channelName = signalValue["Channel"].get<std::string>();

                std::vector<float> data;
                auto measurementData = signalValue["Data"].get<std::vector<int>>();
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

            {
                const std::lock_guard<std::mutex> lock(mutexChunks);
                chunksOutstanding--;
                if (lastChunkSent && chunksOutstanding == 0) {
                    cloudLog(CLOUD_LOG_LEVEL_DEBUG, "Last chunk sent and none outstanding, so closing the stream");

                    closeStream(); // All responses received, shut the stream down
                }
            }
        }
    }
}
