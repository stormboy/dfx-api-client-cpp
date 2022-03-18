// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_LICENSE_TYPES_H
#define DFX_API_CLOUD_LICENSE_TYPES_H

#include "dfx/api/CloudAPI_Export.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dfx::api
{

using License = struct License
{
    std::string id;
    std::string licenseType;
    std::string status;
    std::string expiration;
    std::string key;
    uint64_t maxDevices;
    uint64_t registrationCount;
    uint64_t createdEpochSeconds;

    //  LicenseTypeID
    //  OrganizationID
    //  RemainingMeasurement
    //  SDKPlatforms
    //  Username
};

DFXCLOUD_EXPORT void to_json(nlohmann::json& j, const License& l);
DFXCLOUD_EXPORT void from_json(const nlohmann::json& j, License& l);

} // namespace dfx::api

#endif // DFX_API_CLOUD_LICENSE_TYPES_H
