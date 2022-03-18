// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/websocket/LicenseWebSocket.hpp"
#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/websocket/CloudWebSocket.hpp"

#include "nlohmann/json.hpp"
#include <fmt/format.h>
#include <string>

using namespace dfx::api;
using namespace dfx::api::websocket;
using nlohmann::json;

LicenseWebSocket::LicenseWebSocket(const CloudConfig& config, std::shared_ptr<CloudWebSocket> cloudWebSocket)
    : cloudWebSocket(std::move(cloudWebSocket))
{
}

CloudStatus LicenseWebSocket::list(const CloudConfig& config,
                                   const std::unordered_map<LicenseFilter, std::string>& filters,
                                   uint16_t offset,
                                   std::vector<License>& licenses,
                                   int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(LicenseValidator, list(config, filters, offset, licenses, totalCount));
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "WebSocket", "license list"));
}