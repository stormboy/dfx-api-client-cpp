// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/websocket/MeasurementWebSocket.hpp"
#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/websocket/CloudWebSocket.hpp"

#include "dfx/proto/measurements.pb.h"
#include <google/protobuf/util/json_util.h>

#include "nlohmann/json.hpp"
#include <fmt/format.h>
#include <string>

using namespace dfx::api;
using namespace dfx::api::websocket;
using nlohmann::json;

MeasurementWebSocket::MeasurementWebSocket(const CloudConfig& config, std::shared_ptr<CloudWebSocket> cloudWebSocket)
    : cloudWebSocket(std::move(cloudWebSocket))
{
}

CloudStatus MeasurementWebSocket::list(const CloudConfig& config,
                                       const std::unordered_map<MeasurementFilter, std::string>& filters,
                                       uint16_t offset,
                                       std::vector<Measurement>& measurements,
                                       int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(MeasurementValidator, list(config, filters, offset, measurements, totalCount));

    if (filters.size() > 0) {
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "Unexpected list filter key");
    }

    dfx::proto::measurements::ListRequest request;

    dfx::proto::measurements::ListResponse response;
    auto status = cloudWebSocket->sendMessage(dfx::api::web::Measurements::List, request, response);
    if (status.OK()) {
        for (int index = 0; index < response.values_size(); index++) {
            const auto& pbMeasurement = response.values(index);
            Measurement measurement;
            measurement.id = pbMeasurement.id();
            measurement.userID = pbMeasurement.userid();
            measurement.userProfileID = pbMeasurement.userprofileid();
            measurement.deviceID = pbMeasurement.deviceid();
            measurement.studyID = pbMeasurement.studyid();
            measurement.status = MeasurementStatusMapper::getEnum(pbMeasurement.statusid());
            measurement.deviceVersion = pbMeasurement.deviceversion();
            measurement.createdEpochSeconds = pbMeasurement.created();
            measurement.updatedEpochSeconds = pbMeasurement.updated();
            measurement.userProfileName = pbMeasurement.userprofilename();
            measurement.mode = pbMeasurement.mode();

            if (index == 0) {
                totalCount = pbMeasurement.totalcount();
            }

            measurements.push_back(measurement);
        }
    }

    return CloudStatus(CLOUD_OK);
}

CloudStatus MeasurementWebSocket::retrieve(const CloudConfig& config,
                                           const std::string& measurementID,
                                           Measurement& measurementData)
{
    DFX_CLOUD_VALIDATOR_MACRO(MeasurementValidator, retrieve(config, measurementID, measurementData));

    dfx::proto::measurements::RetrieveRequest request;
    dfx::proto::measurements::RetrieveResponse response;

    request.mutable_params()->set_id(measurementID);

    auto status = cloudWebSocket->sendMessage(dfx::api::web::Measurements::Retrieve, request, response);
    if (status.OK()) {
        measurementData.studyID = response.studyid();
        measurementData.statusID = response.statusid();
        measurementData.deviceVersion = response.deviceversion();
        measurementData.createdEpochSeconds = response.created();
        measurementData.updatedEpochSeconds = response.updated();
        measurementData.id = response.id();
        measurementData.userProfileID = response.username();
        measurementData.userProfileName = response.userprofilename();
        measurementData.mode = response.mode();
        measurementData.region = response.region();

        // The "Files" and "Results" fields if they are present are formatted as google.protobuf.Struct
        // which is ugly to parse in C++ so convert the entire response Message to a JSON object and pull
        // out these two specific fields if present.
        std::string strOutput;
        if (google::protobuf::util::MessageToJsonString(response, &strOutput).ok()) {
            auto j = nlohmann::json::parse(strOutput);
            getStringifedField(j, "Files", measurementData.files);
            getStringifedField(j, "Results", measurementData.results);
        }
    }

    return status;
}

CloudStatus MeasurementWebSocket::retrieveMultiple(const CloudConfig& config,
                                                   const std::vector<std::string>& measurementIDs,
                                                   std::vector<Measurement>& measurements)
{
    DFX_CLOUD_VALIDATOR_MACRO(MeasurementValidator, retrieveMultiple(config, measurementIDs, measurements));

    bool foundAtLeastOne = false;
    for (const auto& measurementID : measurementIDs) {
        Measurement measurement;
        auto status = retrieve(config, measurementID, measurement);
        if (status.OK()) {
            foundAtLeastOne = true;
            measurements.push_back(measurement);
        }
    }

    if (foundAtLeastOne) {
        return CloudStatus(CLOUD_OK);
    } else {
        return CloudStatus(CLOUD_BAD_REQUEST);
    }
}
