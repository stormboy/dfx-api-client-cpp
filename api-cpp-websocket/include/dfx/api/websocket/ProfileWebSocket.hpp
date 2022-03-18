// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_PROFILE_WEBSOCKET_H
#define DFX_API_CLOUD_PROFILE_WEBSOCKET_H

#include "dfx/api/CloudAPI_Export.hpp"
#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/CloudStatus.hpp"
#include "dfx/api/ProfileAPI.hpp"
#include "dfx/api/types/ProfileTypes.hpp"

#include <cstdint>
#include <list>
#include <memory>
#include <string>

namespace dfx::api::websocket
{

class CloudWebSocket;

class ProfileWebSocket : public ProfileAPI
{
public:
    ProfileWebSocket(const CloudConfig& config, std::shared_ptr<CloudWebSocket> cloudWebSocket);

    ~ProfileWebSocket() override = default;

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
    std::shared_ptr<CloudWebSocket> cloudWebSocket;
};

} // namespace dfx::api::websocket

#endif // DFX_API_CLOUD_PROFILE_WEBSOCKET_H
