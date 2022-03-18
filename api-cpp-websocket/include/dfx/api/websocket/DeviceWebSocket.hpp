// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_DEVICE_WEBSOCKET_H
#define DFX_API_CLOUD_DEVICE_WEBSOCKET_H

#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/DeviceAPI.hpp"
#include "dfx/api/websocket/CloudWebSocket.hpp"

#include <memory>

namespace dfx::api::websocket
{

class CloudWebSocket;

class DeviceWebSocket : public DeviceAPI
{
public:
    DeviceWebSocket(const CloudConfig& config, std::shared_ptr<CloudWebSocket> cloudWebSocket);

    ~DeviceWebSocket() override = default;

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
    std::shared_ptr<CloudWebSocket> cloudWebSocket;
};

} // namespace dfx::api::websocket

#endif // DFX_API_CLOUD_DEVICE_WEBSOCKET_H
