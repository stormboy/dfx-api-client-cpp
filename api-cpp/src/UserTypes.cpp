// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/types/UserTypes.hpp"
#include "dfx/api/CloudTypes.hpp"

#include "fmt/format.h"

using namespace dfx::api;

// NOLINTNEXTLINE(cert-err58-cpp)
const std::map<UserStatus, std::string> UserStatusMapper::toString = {
    {UserStatus::UNKNOWN, "UNKNOWN"},
    {UserStatus::ACTIVE, "ACTIVE"},
    {UserStatus::INACTIVE, "INACTIVE"},
    {UserStatus::DELETED, "DELETED"},
};

// NOLINTNEXTLINE(cert-err58-cpp)
const std::map<std::string, UserStatus> UserStatusMapper::toEnum{{"UNKNOWN", UserStatus::UNKNOWN},
                                                                 {"ACTIVE", UserStatus::ACTIVE},
                                                                 {"INACTIVE", UserStatus::INACTIVE},
                                                                 {"DELETED", UserStatus::DELETED}};

UserStatus UserStatusMapper::getEnum(const std::string& type)
{
    try {
        return UserStatusMapper::toEnum.at(type);
    } catch (const std::out_of_range&) {
        return UserStatus::UNKNOWN;
    }
}

std::string UserStatusMapper::getString(UserStatus type)
{
    try {
        return UserStatusMapper::toString.at(type);
    } catch (const std::out_of_range&) {
        return "UNKNOWN";
    }
}

void dfx::api::to_json(nlohmann::json& j, const User& u)
{
    j = nlohmann::json();
    setJSONFieldIfNotDefault(j, "ID", u.id);
    setJSONFieldIfNotDefault(j, "Email", u.email);
    setJSONFieldIfNotDefault(j, "FirstName", u.firstName);
    setJSONFieldIfNotDefault(j, "LastName", u.lastName);
    // status
    setJSONFieldIfNotDefault(j, "Gender", u.gender);
    setJSONFieldIfNotDefault(j, "HeightCM", u.heightCM);
    setJSONFieldIfNotDefault(j, "WeightKG", u.weightKG);
    setJSONFieldIfNotDefault(j, "AvatarURL", u.avatarURL);
    setJSONFieldIfNotDefault(j, "DateOfBirth", u.dateOfBirthEpochSeconds);
    setJSONFieldIfNotDefault(j, "Created", u.createdEpochSeconds);
    setJSONFieldIfNotDefault(j, "Updated", u.updatedEpochSeconds);
    setJSONFieldIfNotDefault(j, "Password", u.password);
    setJSONFieldIfNotDefault(j, "RoleID", u.role);

    setJSONFieldIfNotDefault(j, "OrganizationID", u.organizationID);
    setJSONFieldIfNotDefault(j, "ResetToken", u.resetToken);
    setJSONFieldIfNotDefault(j, "ResetTokenDate", u.resetTokenDate);
    setJSONFieldIfNotDefault(j, "IsVerified", u.isVerified);
    setJSONFieldIfNotDefault(j, "VerificationCode", u.verificationCode);
    setJSONFieldIfNotDefault(j, "PhoneNumber", u.phoneNumber);
    setJSONFieldIfNotDefault(j, "DeviceID", u.deviceID);
    setJSONFieldIfNotDefault(j, "LoginMethod", u.loginMethod);
    setJSONFieldIfNotDefault(j, "SSOID", u.ssoID);
    setJSONFieldIfNotDefault(j, "Region", u.region);
}

void dfx::api::from_json(const nlohmann::json& j, User& u)
{
    getValidField(j, "ID", u.id);
    getValidField(j, "Gender", u.gender);
    getValidField(j, "DateOfBirth", u.dateOfBirthEpochSeconds);
    getValidField(j, "Created", u.createdEpochSeconds);
    getValidField(j, "Updated", u.updatedEpochSeconds);
    getValidField(j, "OrganizationID", u.organizationID);
    getValidField(j, "RoleID", u.role);

    nlohmann::json field;
    if (jsonValidField(j, "StatusID", field)) {
        const std::string& status = field;
        u.status = UserStatusMapper::getEnum(status);
    } else {
        u.status = UserStatus::UNKNOWN;
    }

    getValidField(j, "Email", u.email);
    getValidField(j, "Password", u.password);
    getValidField(j, "FirstName", u.firstName);
    getValidField(j, "LastName", u.lastName);
    getValidField(j, "ResetToken", u.resetToken);
    getValidField(j, "ResetTokenDate", u.resetTokenDate);
    getValidField(j, "AvatarURI", u.avatarURL);
    getValidField(j, "IsVerified", u.isVerified);
    getValidField(j, "VerificationCode", u.verificationCode);
    getValidField(j, "PhoneNumber", u.phoneNumber);
    getValidField(j, "DeviceID", u.deviceID);
    getValidField(j, "HeightCm", u.heightCM);
    getValidField(j, "WeightKg", u.weightKG);
    getValidField(j, "LoginMethod", u.loginMethod);
    getValidField(j, "SSOID", u.ssoID);
    getValidField(j, "Region", u.region);
}
