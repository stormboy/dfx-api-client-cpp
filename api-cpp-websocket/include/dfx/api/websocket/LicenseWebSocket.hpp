// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_LICENSE_WEBSOCKET_H
#define DFX_API_CLOUD_LICENSE_WEBSOCKET_H

#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/CloudStatus.hpp"
#include "dfx/api/LicenseAPI.hpp"
#include "dfx/api/types/LicenseTypes.hpp"

#include <cstdint>
#include <list>
#include <memory>
#include <string>

namespace dfx::api::websocket
{

class CloudWebSocket;

class LicenseWebSocket : public LicenseAPI
{
public:
    LicenseWebSocket(const CloudConfig& config, std::shared_ptr<CloudWebSocket> cloudWebSocket);

    ~LicenseWebSocket() = default;

    CloudStatus list(const CloudConfig& config,
                     const std::unordered_map<LicenseFilter, std::string>& filters,
                     uint16_t offset,
                     std::vector<License>& licenses,
                     int16_t& totalCount) override;

private:
    std::shared_ptr<CloudWebSocket> cloudWebSocket;
};

} // namespace dfx::api::websocket

#endif // DFX_API_CLOUD_LICENSE_WEBSOCKET_H
