// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_PROFILE_WEBSOCKET_JSON_H
#define DFX_API_CLOUD_PROFILE_WEBSOCKET_JSON_H

#include "dfx/api/CloudAPI_Export.hpp"
#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/CloudStatus.hpp"
#include "dfx/api/ProfileAPI.hpp"
#include "dfx/api/types/ProfileTypes.hpp"

#include <cstdint>
#include <list>
#include <memory>
#include <string>

namespace dfx::api::websocket::json
{

class CloudWebSocketJson;

class ProfileWebSocketJson : public ProfileAPI
{
public:
    ProfileWebSocketJson(const CloudConfig& config, std::shared_ptr<CloudWebSocketJson> cloudWebSocketJson);

    ~ProfileWebSocketJson() override = default;

    CloudStatus
    create(const CloudConfig& config, const std::string& name, const std::string& email, Profile& profile) override;

    CloudStatus list(const CloudConfig& config,
                     const std::unordered_map<ProfileFilter, std::string>& filters,
                     uint16_t offset,
                     std::vector<Profile>& profiles,
                     int16_t& totalCount) override;

    CloudStatus retrieve(const CloudConfig& config, const std::string& profileID, Profile& profile) override;

    CloudStatus update(const CloudConfig& config, const Profile& profile) override;

    CloudStatus remove(const CloudConfig& config, const std::string& profileID) override;

private:
    std::shared_ptr<CloudWebSocketJson> cloudWebSocketJson;
};

} // namespace dfx::api::websocket::json

#endif // DFX_API_CLOUD_PROFILE_WEBSOCKET_JSON_H
