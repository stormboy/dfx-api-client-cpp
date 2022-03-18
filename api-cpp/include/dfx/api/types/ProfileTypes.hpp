// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_PROFILE_TYPES_H
#define DFX_API_CLOUD_PROFILE_TYPES_H

#include "dfx/api/CloudAPI_Export.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dfx::api
{

enum class ProfileStatus : std::uint8_t
{
    UNKNOWN = 0,
    ACTIVE,
    INACTIVE,
};

using Profile = struct Profile
{
    std::string id;
    std::string ownerUserEmail;
    std::string name;
    std::string email;
    ProfileStatus status;
    uint32_t measurementCount;
    uint64_t createdEpochSeconds;
    uint64_t updatedEpochSeconds;
};

struct ProfileStatusMapper
{
    static const std::map<ProfileStatus, std::string> toString;
    static const std::map<std::string, ProfileStatus> toEnum;

    DFXCLOUD_EXPORT static ProfileStatus getEnum(const std::string& status);
    DFXCLOUD_EXPORT static std::string getString(ProfileStatus status);
    DFXCLOUD_EXPORT static std::vector<ProfileStatus> getValues();
};

DFXCLOUD_EXPORT void to_json(nlohmann::json& j, const Profile& p);
DFXCLOUD_EXPORT void from_json(const nlohmann::json& j, Profile& p);

} // namespace dfx::api

#endif // DFX_API_CLOUD_PROFILE_TYPES_H
