// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/DeviceAPI.hpp"

using namespace dfx::api;

CloudStatus DeviceAPI::create(const CloudConfig& config,
                              const std::string& name,
                              DeviceType type,
                              const std::string& identifier,
                              const std::string& version,
                              Device& device)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus DeviceAPI::list(const CloudConfig& config,
                            const std::unordered_map<DeviceFilter, std::string>& filters,
                            uint16_t offset,
                            std::vector<Device>& devices,
                            int16_t& totalCount)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus DeviceAPI::retrieve(const CloudConfig& config, const std::string& deviceID, Device& device)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

// Polyfill for web based on retrieve, gRPC it is natively implemented
CloudStatus DeviceAPI::retrieveMultiple(const CloudConfig& config,
                                        const std::vector<std::string>& deviceIDs,
                                        std::vector<Device>& devices)
{
    // Validate will occur by first retrieve call

    std::vector<Device> deviceList;
    for (const auto& id : deviceIDs) {
        Device device;
        auto status = retrieve(config, id, device);
        if (status.OK()) {
            deviceList.push_back(device);
        } else {
            return status;
        }
    }

    // Copy all the items we retrieved - this ensures devices state consistent on failure
    // and allows client to pass existing items in list without us clearing.
    devices.insert(devices.end(), deviceList.begin(), deviceList.end());
    return CloudStatus(CLOUD_OK);
}

CloudStatus DeviceAPI::update(const CloudConfig& config, const Device& device)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus DeviceAPI::remove(const CloudConfig& config, const std::string& deviceID)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}
