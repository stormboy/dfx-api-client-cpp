// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/ProfileAPI.hpp"

using namespace dfx::api;

CloudStatus
ProfileAPI::create(const CloudConfig& config, const std::string& name, const std::string& email, Profile& profile)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus ProfileAPI::list(const CloudConfig& config,
                             const std::unordered_map<ProfileFilter, std::string>& filters,
                             uint16_t offset,
                             std::vector<Profile>& profiles,
                             int16_t& totalCount)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus ProfileAPI::retrieve(const CloudConfig& config, const std::string& profileID, Profile& profile)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus ProfileAPI::update(const CloudConfig& config, const Profile& profile)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus ProfileAPI::remove(const CloudConfig& config, const std::string& profileID)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}