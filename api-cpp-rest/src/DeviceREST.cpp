// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/rest/DeviceREST.hpp"
#include "dfx/api/rest/CloudREST.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

#include "nlohmann/json.hpp"
#include <sstream>
#include <string>

using namespace dfx::api;
using namespace dfx::api::rest;

CloudStatus DeviceREST::create(const CloudConfig& config,
                               const std::string& name,
                               DeviceType type,
                               const std::string& identifier,
                               const std::string& version,
                               Device& device)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, create(config, name, type, identifier, version, device));

    nlohmann::json request = {{"Name", name},
                              {"DeviceTypeID", DeviceTypeMapper::toString.at(type)},
                              {"Identifier", identifier},
                              {"Version", version}};
    nlohmann::json response;

    auto result = CloudREST::performRESTCall(config, web::Devices::Create, config.authToken, {}, request, response);
    if (result.OK()) {
        std::string deviceID = response["ID"].get<std::string>();
        return retrieve(config, deviceID, device);
    }

    return result;
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
    std::stringstream urlQuery;
    urlQuery << "Offset=" << offset << "&Limit=" << config.listLimit;
    for (auto& filter : filters) {
        switch (filter.first) {
            case DeviceFilter::Date:
                urlQuery << "&Date=" << filter.second;
                break;
            case DeviceFilter::EndDate:
                urlQuery << "&EndDate=" << filter.second;
                break;
            case DeviceFilter::Name:
                urlQuery << "&Name=" << filter.second;
                break;
            case DeviceFilter::Version:
                urlQuery << "&Version=" << filter.second;
                break;
            case DeviceFilter::StatusID:
                urlQuery << "&StatusID=" << filter.second;
                break;
            case DeviceFilter::DeviceTypeID:
                urlQuery << "&DeviceTypeID=" << filter.second;
                break;
            case DeviceFilter::LicenseID:
                urlQuery << "&LicenseID=" << filter.second;
                break;
            case DeviceFilter::SortOrder:
                urlQuery << "&SortOrder=" << filter.second;
                break;
            default:
                return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "Unexpected list filter key");
        }
    }

    nlohmann::json request;
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/devices/list
    auto result =
        CloudREST::performRESTCall(config, web::Devices::List, config.authToken, {}, urlQuery.str(), request, response);
    if (result.OK()) {
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

    return result;
}

CloudStatus DeviceREST::retrieve(const CloudConfig& config, const std::string& deviceID, Device& device)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, retrieve(config, deviceID, deviceInfo));

    nlohmann::json request;
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/devices/retrieve
    auto result =
        CloudREST::performRESTCall(config, web::Devices::Retrieve, config.authToken, {deviceID}, request, response);

    if (result.OK()) {
        device = response;
        device.id = deviceID;
    }

    return result;
}

CloudStatus DeviceREST::update(const CloudConfig& config, const Device& device)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, update(config, device));

    nlohmann::json request = device;
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/devices/update
    return CloudREST::performRESTCall(config, web::Devices::Update, config.authToken, {device.id}, request, response);
}

CloudStatus DeviceREST::remove(const CloudConfig& config, const std::string& deviceID)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, remove(config, deviceID));

    nlohmann::json request;
    nlohmann::json response;
    return CloudREST::performRESTCall(config, web::Devices::Remove, config.authToken, {deviceID}, request, response);
}
