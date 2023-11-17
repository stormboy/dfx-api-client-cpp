// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/websocket/json/MeasurementWebSocketJson.hpp"
#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/websocket/json/CloudWebSocketJson.hpp"

#include "dfx/proto/measurements.pb.h"
#include <google/protobuf/util/json_util.h>

#include "nlohmann/json.hpp"
#include <string>

using namespace dfx::api;
using namespace dfx::api::websocket::json;

MeasurementWebSocketJson::MeasurementWebSocketJson(const CloudConfig& config,
                                                   std::shared_ptr<CloudWebSocketJson> cloudWebSocketJson)
    : cloudWebSocketJson(std::move(cloudWebSocketJson))
{
}

CloudStatus MeasurementWebSocketJson::list(const CloudConfig& config,
                                           const std::unordered_map<MeasurementFilter, std::string>& filters,
                                           uint16_t offset,
                                           std::vector<Measurement>& measurements,
                                           int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(MeasurementValidator, list(config, filters, offset, measurements, totalCount));

    nlohmann::json request;
    nlohmann::json response;

    auto fullObject = false;

    nlohmann::json query;
    query["Offset"] = offset;
    query["Limit"] = config.listLimit;

    for (auto& filter : filters) {
        switch (filter.first) {
            case MeasurementFilter::StartDate:
                query["Date"] = filter.second;
                break;
            case MeasurementFilter::EndDate:
                query["EndDate"] = filter.second;
                break;
            case MeasurementFilter::UserProfileId:
                query["UserProfileID"] = filter.second;
                break;
            case MeasurementFilter::UserProfileName:
                query["UserProfileName"] = filter.second;
                break;
            case MeasurementFilter::StudyId:
                query["StudyID"] = filter.second;
                break;
            case MeasurementFilter::StatusId:
                query["StatusID"] = filter.second;
                break;
            case MeasurementFilter::PartnerId:
                query["PartnerID"] = filter.second;
                break;
            case MeasurementFilter::FullObject:
                fullObject = true;
                break;
            default:
                return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "Unexpected list filter key");
        }
    }

    // https://dfxapiversion10.docs.apiary.io/#reference/0/measurements/list
    auto result = cloudWebSocketJson->sendMessageJson(config, web::Measurements::List, {}, {}, request, response);

    if (result.OK()) {
        std::vector<Measurement> partialObjects = response;

        if (partialObjects.size() > 0) {
            // First element assuming there is one will have a TotalCount field which makes for
            // a non-uniform JSON schema so custom decode here
            if (response[0].contains("TotalCount")) {
                totalCount = response[0]["TotalCount"];
            }
        }

        if (!fullObject) {
            measurements.insert(measurements.end(), partialObjects.begin(), partialObjects.end());
        } else {
            std::vector<std::string> measurementIDs;
            measurementIDs.reserve(partialObjects.size());
            for (auto& measurement : partialObjects) {
                // Throwaway everything but the measurement ID, we want full records
                measurementIDs.push_back(measurement.id);
            }

            return retrieveMultiple(config, measurementIDs, measurements);
        }
    }

    return result;
}

CloudStatus MeasurementWebSocketJson::retrieve(const CloudConfig& config,
                                               const std::string& measurementID,
                                               Measurement& measurementData)
{
    DFX_CLOUD_VALIDATOR_MACRO(MeasurementValidator, retrieve(config, measurementID, measurementData));

    nlohmann::json request;
    nlohmann::json response;

    nlohmann::json params = {{"ID", measurementID}};

    // https://dfxapiversion10.docs.apiary.io/#reference/0/measurements/retrieve
    auto result =
        cloudWebSocketJson->sendMessageJson(config, web::Measurements::Retrieve, params, {}, request, response);

    if (result.OK()) {
        measurementData = response;
    }

    return result;
}

CloudStatus MeasurementWebSocketJson::retrieveMultiple(const CloudConfig& config,
                                                       const std::vector<std::string>& measurementIDs,
                                                       std::vector<Measurement>& measurements)
{
    DFX_CLOUD_VALIDATOR_MACRO(MeasurementValidator, retrieveMultiple(config, measurementIDs, measurements));

    bool foundAtLeastOne = false;
    for (const auto& measurementID : measurementIDs) {
        Measurement measurement;
        auto result = retrieve(config, measurementID, measurement);
        if (result.OK()) {
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
