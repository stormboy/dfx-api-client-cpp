// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/rest/DeviceREST.hpp"
#include "dfx/api/rest/CloudREST.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

#include "nlohmann/json.hpp"
#include <fmt/format.h>
#include <string>

using namespace dfx::api;
using namespace dfx::api::rest;
using nlohmann::json;

CloudStatus DeviceREST::create(const CloudConfig& config,
                               const std::string& name,
                               DeviceType type,
                               const std::string& identifier,
                               const std::string& version,
                               Device& device)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, create(config, name, type, identifier, version, device));

    json response, request = {{"Name", name},
                              {"DeviceTypeID", DeviceTypeMapper::toString.at(type)},
                              {"Identifier", identifier},
                              {"Version", version}};
    auto status = CloudREST::performRESTCall(config, web::Devices::Create, config.authToken, {}, request, response);
    if (status.OK()) {
        std::string deviceID = response["ID"].get<std::string>();
        return retrieve(config, deviceID, device);
    }
    return status;
}

CloudStatus DeviceREST::list(const CloudConfig& config,
                             const std::unordered_map<DeviceFilter, std::string>& filters,
                             uint16_t offset,
                             std::vector<Device>& deviceList,
                             int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, list(config, filters, offset, deviceList, totalCount));

    totalCount = -1; // Return unknown -1, zero would be a literal zero

    // REST: https://dfxapiversion10.docs.apiary.io/#reference/0/devices/list
    auto fullObject = false;
    std::stringstream urlQuery;
    urlQuery << "Offset=" << offset << "&Limit=" << config.listLimit;
    for (auto& filter : filters) {
        switch (filter.first) {
            case DeviceFilter::StatusID:
                urlQuery << "&Date=" << filter.second;
                break;
            case DeviceFilter::Unique:
                urlQuery << "&EndDate=" << filter.second;
                break;
            case DeviceFilter::DeviceTypeID:
                urlQuery << "&UserProfileID=" << filter.second;
                break;
            case DeviceFilter::Name:
                urlQuery << "&UserProfileName=" << filter.second;
                break;
            case DeviceFilter::Version:
                urlQuery << "&StudyID=" << filter.second;
                break;
            case DeviceFilter::Date:
                urlQuery << "&StatusID=" << filter.second;
                break;
            case DeviceFilter::EndDate:
                urlQuery << "&PartnerID=" << filter.second;
                break;
            case DeviceFilter::FullObject:
                fullObject = true;
                break;
            default:
                return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "Unexpected list filter key");
        }
    }

    json response, request;
    auto status =
        CloudREST::performRESTCall(config, web::Devices::List, config.authToken, {}, urlQuery.str(), request, response);
    if (status.OK()) {
        std::vector<Device> devices = response;

        // Append everything we just read to the deviceList
        deviceList.insert(deviceList.end(), devices.begin(), devices.end());

        if (devices.size() > 0) {
            // First element assuming there is one will have a TotalCount field which makes for
            // a non-uniform JSON schema so custom decode here
            if (response[0].contains("TotalCount")) {
                totalCount = response[0]["TotalCount"];
            }
        }
    }

    return status;
}

CloudStatus DeviceREST::retrieve(const CloudConfig& config, const std::string& deviceID, Device& deviceInfo)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, retrieve(config, deviceID, deviceInfo));

    json response, request;
    auto status =
        CloudREST::performRESTCall(config, web::Devices::Retrieve, config.authToken, {deviceID}, request, response);

    if (status.OK()) {
        deviceInfo = response;
        deviceInfo.id = deviceID;
    }
    return status;
}

CloudStatus DeviceREST::update(const CloudConfig& config, const Device& device)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, update(config, device));

    json response, request = device;
    return CloudREST::performRESTCall(config, web::Devices::Update, config.authToken, {device.id}, request, response);
}

CloudStatus DeviceREST::remove(const CloudConfig& config, const std::string& deviceID)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, remove(config, deviceID));

    json response, request;
    return CloudREST::performRESTCall(config, web::Devices::Remove, config.authToken, {deviceID}, request, response);
}
