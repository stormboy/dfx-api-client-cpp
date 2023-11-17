// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_DEVICE_WEBSOCKET_JSON_H
#define DFX_API_CLOUD_DEVICE_WEBSOCKET_JSON_H

#include "CloudWebSocketJson.hpp"
#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/DeviceAPI.hpp"

#include <memory>

namespace dfx::api::websocket::json
{

class CloudWebSocketJson;

class DeviceWebSocketJson : public DeviceAPI
{
public:
    DeviceWebSocketJson(const CloudConfig& config, std::shared_ptr<CloudWebSocketJson> cloudWebSocketJson);

    ~DeviceWebSocketJson() override = default;

    CloudStatus create(const CloudConfig& config,
                       const std::string& name,
                       DeviceType type,
                       const std::string& identifier,
                       const std::string& version,
                       Device& device) override;

    CloudStatus list(const CloudConfig& config,
                     const std::unordered_map<DeviceFilter, std::string>& filters,
                     uint16_t offset,
                     std::vector<Device>& deviceList,
                     int16_t& totalCount) override;

    CloudStatus retrieve(const CloudConfig& config, const std::string& deviceID, Device& device) override;

    CloudStatus update(const CloudConfig& config, const Device& device) override;

    CloudStatus remove(const CloudConfig& config, const std::string& deviceID) override;

private:
    std::shared_ptr<CloudWebSocketJson> cloudWebSocketJson;
};

} // namespace dfx::api::websocket::json

#endif // DFX_API_CLOUD_DEVICE_WEBSOCKET_JSON_H
