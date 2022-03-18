// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/types/ProfileTypes.hpp"
#include "dfx/api/CloudLog.hpp"
#include "dfx/api/CloudTypes.hpp"

#include "fmt/format.h"

using namespace dfx::api;

const std::map<ProfileStatus, std::string> ProfileStatusMapper::toString{
    {ProfileStatus::UNKNOWN, "UNKNOWN"}, {ProfileStatus::ACTIVE, "ACTIVE"}, {ProfileStatus::INACTIVE, "INACTIVE"}};

const std::map<std::string, ProfileStatus> ProfileStatusMapper::toEnum{
    {"UNKNOWN", ProfileStatus::UNKNOWN}, {"ACTIVE", ProfileStatus::ACTIVE}, {"INACTIVE", ProfileStatus::INACTIVE}};

ProfileStatus ProfileStatusMapper::getEnum(const std::string& status)
{
    try {
        return ProfileStatusMapper::toEnum.at(status);
    } catch (const std::out_of_range&) {
        cloudLog(CLOUD_LOG_LEVEL_WARNING, "Unexpected ProfileStatus: %s\n", status.c_str());
        return ProfileStatus::UNKNOWN;
    }
}

std::string ProfileStatusMapper::getString(ProfileStatus status)
{
    try {
        return ProfileStatusMapper::toString.at(status);
    } catch (const std::out_of_range&) {
        cloudLog(CLOUD_LOG_LEVEL_WARNING, "Unexpected ProfileStatus: %d\n", static_cast<uint16_t>(status));
        return "UNKNOWN";
    }
}

static std::vector<ProfileStatus> valid_values = {ProfileStatus::ACTIVE, ProfileStatus::INACTIVE};
std::vector<ProfileStatus> ProfileStatusMapper::getValues()
{
    return valid_values;
}

void dfx::api::to_json(nlohmann::json& j, const Profile& p)
{
    j = nlohmann::json();
    setJSONFieldIfNotDefault(j, "ID", p.id);
    setJSONFieldIfNotDefault(j, "Name", p.name);
    setJSONFieldIfNotDefault(j, "Email", p.email);
    setJSONFieldIfNotDefault(j, "MeasurementCount", p.measurementCount);
    setJSONFieldIfNotDefault(j, "Created", p.createdEpochSeconds);
    setJSONFieldIfNotDefault(j, "Updated", p.updatedEpochSeconds);
    setJSONFieldIfNotDefault(j, "OwnerUserEmail", p.ownerUserEmail);

    if (p.status != ProfileStatus::UNKNOWN) {
        j["Status"] = ProfileStatusMapper::getString(p.status);
    }
}

void dfx::api::from_json(const nlohmann::json& j, Profile& p)
{
    getValidField(j, "ID", p.id);
    getValidField(j, "Name", p.name);
    getValidField(j, "Email", p.email);

    nlohmann::json field;
    if (jsonValidField(j, "Status", field)) {
        const std::string& statusType = field;
        p.status = ProfileStatusMapper::getEnum(statusType);
    } else {
        p.status = ProfileStatus::UNKNOWN;
    }

    getValidField(j, "MeasurementCount", p.measurementCount);
    getValidField(j, "Created", p.createdEpochSeconds);
    getValidField(j, "Updated", p.updatedEpochSeconds);

    // REST retrieve() does not seem to have this value
    getValidField(j, "OwnerUserEmail", p.ownerUserEmail);
}
