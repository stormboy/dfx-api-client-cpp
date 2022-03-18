// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_DEVICE_GRPC_H
#define DFX_API_CLOUD_DEVICE_GRPC_H

#include "dfx/api/DeviceAPI.hpp"
#include "dfx/devices/v2/devices.grpc.pb.h"

namespace dfx::api::grpc
{

class CloudGRPC;

class DeviceGRPC : public DeviceAPI
{
public:
    DeviceGRPC(const CloudConfig& config, const std::shared_ptr<CloudGRPC>& cloudGRPC);

    ~DeviceGRPC() override = default;

    CloudStatus list(const CloudConfig& config,
                     const std::unordered_map<DeviceFilter, std::string>& filters,
                     uint16_t offset,
                     std::vector<Device>& devices,
                     int16_t& totalCount) override;

    CloudStatus retrieve(const CloudConfig& config, const std::string& deviceID, Device& device) override;

    CloudStatus retrieveMultiple(const CloudConfig& config,
                                 const std::vector<std::string>& deviceIDs,
                                 std::vector<Device>& devices) override;

    CloudStatus update(const CloudConfig& config, const Device& device) override;

    CloudStatus remove(const CloudConfig& config, const std::string& deviceID) override;

private:
    std::unique_ptr<dfx::devices::v2::API::Stub> grpcDeviceStub;
};

} // namespace dfx::api::grpc

#endif // DFX_API_CLOUD_DEVICE_GRPC_H
