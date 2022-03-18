// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_ORGANIZATION_TYPES_H
#define DFX_API_CLOUD_ORGANIZATION_TYPES_H

#include "dfx/api/CloudAPI_Export.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dfx::api
{

enum class OrganizationStatus : std::int8_t
{
    UNKNOWN = 0,
    ACTIVE = 1,
    INACTIVE = 2,
    DELETED = 3
};

using Organization = struct Organization
{
    std::string id;
    std::string name;
    std::string identifier;
    OrganizationStatus status;
    std::string publicKey;
    std::string logo;
    std::string contact;
    std::string email;
    std::string address;
    uint64_t createdEpochSeconds;
    uint64_t updatedEpochSeconds;
};

struct OrganizationStatusMapper
{
    static const std::map<OrganizationStatus, std::string> toString;
    static const std::map<std::string, OrganizationStatus> toEnum;

    DFXCLOUD_EXPORT static OrganizationStatus getEnum(const std::string& status);
    DFXCLOUD_EXPORT static std::string getString(OrganizationStatus status);
};

DFXCLOUD_EXPORT void to_json(nlohmann::json& j, const Organization& o);
DFXCLOUD_EXPORT void from_json(const nlohmann::json& j, Organization& o);

} // namespace dfx::api

#endif // DFX_API_CLOUD_ORGANIZATION_TYPES_H
