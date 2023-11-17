// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/websocket/json/DeviceWebSocketJson.hpp"

#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/websocket/json/CloudWebSocketJson.hpp"

#include "fmt/format.h"
#include "nlohmann/json.hpp"

using namespace dfx::api;
using namespace dfx::api::websocket::json;

DeviceWebSocketJson::DeviceWebSocketJson(const CloudConfig& config,
                                         std::shared_ptr<CloudWebSocketJson> cloudWebSocketJson)
    : cloudWebSocketJson(std::move(cloudWebSocketJson))
{
}

CloudStatus DeviceWebSocketJson::create(const CloudConfig& config,
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

    // https://dfxapiversion10.docs.apiary.io/#reference/0/devices/create
    auto result = cloudWebSocketJson->sendMessageJson(config, web::Devices::Create, {}, {}, request, response);
    if (result.OK()) {
        std::string deviceID = response["ID"].get<std::string>();
        return retrieve(config, deviceID, device);
    }

    return result;
}

CloudStatus DeviceWebSocketJson::list(const CloudConfig& config,
                                      const std::unordered_map<DeviceFilter, std::string>& filters,
                                      uint16_t offset,
                                      std::vector<Device>& deviceList,
                                      int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, list(config, filters, offset, deviceList, totalCount));

    totalCount = -1; // Return unknown -1, zero would be a literal zero

    // REST: https://dfxapiversion10.docs.apiary.io/#reference/0/devices/list
    auto fullObject = false;

    nlohmann::json request;
    nlohmann::json response;

    nlohmann::json query;
    query["Offset"] = offset;
    query["Limit"] = config.listLimit;
    for (auto& filter : filters) {
        switch (filter.first) {
            case DeviceFilter::Date:
                query["Date"] = filter.second;
                break;
            case DeviceFilter::EndDate:
                query["EndDate"] = filter.second;
                break;
            case DeviceFilter::Name:
                query["Name"] = filter.second;
                break;
            case DeviceFilter::Version:
                query["Version"] = filter.second;
                break;
            case DeviceFilter::StatusID:
                query["StatusID"] = filter.second;
                break;
            case DeviceFilter::DeviceTypeID:
                query["DeviceTypeID"] = filter.second;
                break;
            case DeviceFilter::LicenseID:
                query["LicenseID"] = filter.second;
                break;
            case DeviceFilter::SortOrder:
                query["SortOrder"] = filter.second;
                break;
            default:
                return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "Unexpected list filter key");
        }
    }

    // https://dfxapiversion10.docs.apiary.io/#reference/0/devices/list
    auto result = cloudWebSocketJson->sendMessageJson(config, web::Devices::List, {}, query, request, response);
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

CloudStatus DeviceWebSocketJson::retrieve(const CloudConfig& config, const std::string& deviceID, Device& device)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, retrieve(config, deviceID, device));

    nlohmann::json request;
    nlohmann::json response;

    nlohmann::json params;
    params["ID"] = deviceID;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/devices/retrieve
    auto result = cloudWebSocketJson->sendMessageJson(config, web::Devices::Retrieve, params, {}, request, response);

    if (result.OK()) {
        device = response;
        device.id = deviceID;
    }
    return result;
}

CloudStatus DeviceWebSocketJson::update(const CloudConfig& config, const Device& device)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, update(config, device));

    nlohmann::json request = device;
    nlohmann::json response;

    nlohmann::json params;
    params["ID"] = device.id;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/devices/update
    return cloudWebSocketJson->sendMessageJson(config, web::Devices::Update, params, {}, request, response);
}

CloudStatus DeviceWebSocketJson::remove(const CloudConfig& config, const std::string& deviceID)
{
    // No longer supported but to avoid runtime breaking clients, just return OK
    return CloudStatus(CLOUD_OK);
}