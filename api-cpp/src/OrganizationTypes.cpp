// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/types/OrganizationTypes.hpp"
#include "dfx/api/CloudLog.hpp"
#include "dfx/api/CloudTypes.hpp"

#include "fmt/format.h"

using namespace dfx::api;

const std::map<OrganizationStatus, std::string> OrganizationStatusMapper::toString{
    {OrganizationStatus::UNKNOWN, "UNKNOWN"},
    {OrganizationStatus::ACTIVE, "ACTIVE"},
    {OrganizationStatus::INACTIVE, "INACTIVE"},
    {OrganizationStatus::DELETED, "DELETED"}};

const std::map<std::string, OrganizationStatus> OrganizationStatusMapper::toEnum{
    {"UNKNOWN", OrganizationStatus::UNKNOWN},
    {"ACTIVE", OrganizationStatus::ACTIVE},
    {"INACTIVE", OrganizationStatus::INACTIVE},
    {"DELETED", OrganizationStatus::DELETED}};

OrganizationStatus OrganizationStatusMapper::getEnum(const std::string& status)
{
    try {
        return OrganizationStatusMapper::toEnum.at(status);
    } catch (const std::out_of_range&) {
        cloudLog(CLOUD_LOG_LEVEL_WARNING, "Unexpected OrganizationStatus: %s\n", status.c_str());
        return OrganizationStatus::UNKNOWN;
    }
}

std::string OrganizationStatusMapper::getString(OrganizationStatus status)
{
    try {
        return OrganizationStatusMapper::toString.at(status);
    } catch (const std::out_of_range&) {
        cloudLog(CLOUD_LOG_LEVEL_WARNING, "Unexpected OrganizationStatus: %d\n", static_cast<uint16_t>(status));
        return "UNKNOWN";
    }
}

void dfx::api::to_json(nlohmann::json& j, const Organization& o)
{
    j = nlohmann::json();
    setJSONFieldIfNotDefault(j, "ID", o.id);
    setJSONFieldIfNotDefault(j, "Name", o.name);
    setJSONFieldIfNotDefault(j, "Identifier", o.identifier);
    setJSONFieldIfNotDefault(j, "PublicKey", o.publicKey);
    setJSONFieldIfNotDefault(j, "Contact", o.contact);
    setJSONFieldIfNotDefault(j, "Email", o.email);
    setJSONFieldIfNotDefault(j, "Address", o.address);
    setJSONFieldIfNotDefault(j, "Created", o.createdEpochSeconds);
    setJSONFieldIfNotDefault(j, "Updated", o.updatedEpochSeconds);

    if (o.status != OrganizationStatus::UNKNOWN) {
        j["StatusID"] = OrganizationStatusMapper::getString(o.status);
    }
}

void dfx::api::from_json(const nlohmann::json& j, Organization& o)
{
    getValidField(j, "ID", o.id);
    getValidField(j, "Name", o.name);
    getValidField(j, "Identifier", o.identifier);
    getValidField(j, "PublicKey", o.publicKey);
    getValidField(j, "Contact", o.contact);
    getValidField(j, "Email", o.email);
    getValidField(j, "Address", o.address);
    getValidField(j, "Created", o.createdEpochSeconds);
    getValidField(j, "Updated", o.updatedEpochSeconds);

    nlohmann::json field;
    if (jsonValidField(j, "StatusID", field)) {
        const std::string& statusType = field;
        o.status = OrganizationStatusMapper::getEnum(statusType);
    } else {
        o.status = OrganizationStatus::UNKNOWN;
    }
}
