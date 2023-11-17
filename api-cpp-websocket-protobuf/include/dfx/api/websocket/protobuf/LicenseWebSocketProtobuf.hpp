// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_LICENSE_WEBSOCKET_PROTOBUF_H
#define DFX_API_CLOUD_LICENSE_WEBSOCKET_PROTOBUF_H

#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/CloudStatus.hpp"
#include "dfx/api/LicenseAPI.hpp"
#include "dfx/api/types/LicenseTypes.hpp"

#include <cstdint>
#include <list>
#include <memory>
#include <string>

namespace dfx::api::websocket::protobuf
{

class CloudWebSocketProtobuf;

class LicenseWebSocketProtobuf : public LicenseAPI
{
public:
    LicenseWebSocketProtobuf(const CloudConfig& config, std::shared_ptr<CloudWebSocketProtobuf> cloudWebSocketProtobuf);

    ~LicenseWebSocketProtobuf() = default;

    CloudStatus list(const CloudConfig& config,
                     const std::unordered_map<LicenseFilter, std::string>& filters,
                     uint16_t offset,
                     std::vector<License>& licenses,
                     int16_t& totalCount) override;

private:
    std::shared_ptr<CloudWebSocketProtobuf> cloudWebSocketProtobuf;
};

} // namespace dfx::api::websocket::protobuf

#endif // DFX_API_CLOUD_LICENSE_WEBSOCKET_PROTOBUF_H
