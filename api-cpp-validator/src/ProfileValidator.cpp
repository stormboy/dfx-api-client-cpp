// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/validator/ProfileValidator.hpp"
#include "CloudValidatorMacros.hpp"

using namespace dfx::api;
using namespace dfx::api::validator;

const ProfileValidator& ProfileValidator::instance()
{
    static const ProfileValidator instance;
    return instance;
}

CloudStatus
ProfileValidator::create(const CloudConfig& config, const std::string& name, const std::string& email, Profile& profile)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(name);
    MACRO_RETURN_ERROR_IF_EMPTY(email);
    return CloudStatus(CLOUD_OK);
}

CloudStatus ProfileValidator::list(const CloudConfig& config,
                                   const std::unordered_map<ProfileFilter, std::string>& filters,
                                   uint16_t offset,
                                   std::vector<Profile>& profiles,
                                   int16_t& totalCount)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    return CloudStatus(CLOUD_OK);
}

CloudStatus ProfileValidator::retrieve(const CloudConfig& config, const std::string& profileID, Profile& profile)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(profileID);
    return CloudStatus(CLOUD_OK);
}

CloudStatus ProfileValidator::update(const CloudConfig& config, const Profile& profile)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(profile.id);
    MACRO_RETURN_ERROR_IF_EMPTY(profile.name);
    MACRO_RETURN_ERROR_IF_EMPTY(profile.email);
    return CloudStatus(CLOUD_OK);
}

CloudStatus ProfileValidator::remove(const CloudConfig& config, const std::string& profileID)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(profileID);
    return CloudStatus(CLOUD_OK);
}