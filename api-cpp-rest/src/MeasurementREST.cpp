// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/rest/MeasurementREST.hpp"
#include "dfx/api/rest/CloudREST.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

#include "nlohmann/json.hpp"
#include <fmt/format.h>
#include <sstream>
#include <string>

using namespace dfx::api;
using namespace dfx::api::rest;
using nlohmann::json;

CloudStatus MeasurementREST::list(const CloudConfig& config,
                                  const std::unordered_map<MeasurementFilter, std::string>& filters,
                                  uint16_t offset,
                                  std::vector<Measurement>& measurements,
                                  int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(MeasurementValidator, list(config, filters, offset, measurements, totalCount));

    totalCount = -1; // Return unknown -1, zero would be a literal zero

    // REST: https://dfxapiversion10.docs.apiary.io/#reference/0/measurements/list
    auto fullObject = false;
    std::stringstream urlQuery;
    urlQuery << "Offset=" << offset << "&Limit=" << config.listLimit;
    for (auto& filter : filters) {
        switch (filter.first) {
            case MeasurementFilter::StartDate:
                urlQuery << "&Date=" << filter.second;
                break;
            case MeasurementFilter::EndDate:
                urlQuery << "&EndDate=" << filter.second;
                break;
            case MeasurementFilter::UserProfileId:
                urlQuery << "&UserProfileID=" << filter.second;
                break;
            case MeasurementFilter::UserProfileName:
                urlQuery << "&UserProfileName=" << filter.second;
                break;
            case MeasurementFilter::StudyId:
                urlQuery << "&StudyID=" << filter.second;
                break;
            case MeasurementFilter::StatusId:
                urlQuery << "&StatusID=" << filter.second;
                break;
            case MeasurementFilter::PartnerId:
                urlQuery << "&PartnerID=" << filter.second;
                break;
            case MeasurementFilter::FullObject:
                fullObject = true;
                break;
            default:
                return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "Unexpected list filter key");
        }
    }

    json response, request;
    auto status = CloudREST::performRESTCall(
        config, web::Measurements::List, config.authToken, {}, urlQuery.str(), request, response);
    if (status.OK()) {
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
    return status;
}

CloudStatus MeasurementREST::retrieve(const CloudConfig& config,
                                      const std::string& measurementID,
                                      Measurement& measurementData)
{
    DFX_CLOUD_VALIDATOR_MACRO(MeasurementValidator, retrieve(config, measurementID, measurementData));

    json response, request;
    auto status = CloudREST::performRESTCall(
        config, web::Measurements::Retrieve, config.authToken, {measurementID}, request, response);

    if (status.OK()) {
        measurementData = response;
    }
    return status;
}
