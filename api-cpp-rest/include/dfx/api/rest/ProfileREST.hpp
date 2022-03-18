// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_PROFILE_REST_H
#define DFX_API_CLOUD_PROFILE_REST_H

#include "dfx/api/ProfileAPI.hpp"

namespace dfx::api::rest
{

class CloudREST;

class ProfileREST : public ProfileAPI
{
public:
    ProfileREST() = default;

    ~ProfileREST() override = default;

    virtual CloudStatus
    create(const CloudConfig& config, const std::string& name, const std::string& email, Profile& profile) override;

    virtual CloudStatus list(const CloudConfig& config,
                             const std::unordered_map<ProfileFilter, std::string>& filters,
                             uint16_t offset,
                             std::vector<Profile>& profiles,
                             int16_t& totalCount) override;

    CloudStatus retrieve(const CloudConfig& config, const std::string& profileID, Profile& profile) override;

    CloudStatus update(const CloudConfig& config, const Profile& profile) override;

    CloudStatus remove(const CloudConfig& config, const std::string& profileID) override;
};

} // namespace dfx::api::rest

#endif // DFX_API_CLOUD_PROFILE_REST_H
