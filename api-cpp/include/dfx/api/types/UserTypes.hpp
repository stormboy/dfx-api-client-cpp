// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_USER_TYPES_H
#define DFX_API_CLOUD_USER_TYPES_H

#include "dfx/api/CloudAPI_Export.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dfx::api
{

enum class UserStatus : std::uint8_t
{
    UNKNOWN = 0,
    ACTIVE,
    INACTIVE,
    DELETED
};

using User = struct User
{
    std::string id;
    std::string email;
    std::string firstName;
    std::string lastName;
    UserStatus status;
    std::string gender;
    uint16_t heightCM;
    uint16_t weightKG;
    std::string avatarURL;
    uint64_t createdEpochSeconds;
    uint64_t updatedEpochSeconds;
    std::string password;
    std::string role;

    std::string organizationID;
    std::string resetToken;
    std::string resetTokenDate;
    bool isVerified;
    std::string verificationCode;
    std::string phoneNumber;
    std::string deviceID;
    std::string loginMethod;
    std::string ssoID;
    std::string region;
    std::string dateOfBirth;

    std::string passwordSetDate;
    bool mfaEnabled;
};

using UserRole = struct UserRole
{
    std::string id;
    std::string name;
    std::string description;
    std::string organization;
};

struct UserStatusMapper
{
    static const std::map<UserStatus, std::string> toString;
    static const std::map<std::string, UserStatus> toEnum;

    DFXCLOUD_EXPORT static UserStatus getEnum(const std::string& status);

    DFXCLOUD_EXPORT static std::string getString(UserStatus status);
};

DFXCLOUD_EXPORT void from_json(const nlohmann::json& j, User& u);
DFXCLOUD_EXPORT void to_json(nlohmann::json& j, const User& u);

} // namespace dfx::api

#endif // DFX_API_CLOUD_USER_TYPES_H
