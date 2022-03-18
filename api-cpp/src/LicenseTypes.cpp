// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/types/LicenseTypes.hpp"
#include "dfx/api/CloudTypes.hpp"

using namespace dfx::api;

void dfx::api::to_json(nlohmann::json& j, const License& l)
{
    j = nlohmann::json();
    setJSONFieldIfNotDefault(j, "ID", l.id);
    setJSONFieldIfNotDefault(j, "LicenseType", l.licenseType);
    setJSONFieldIfNotDefault(j, "StatusID", l.status);
    setJSONFieldIfNotDefault(j, "Expiration", l.expiration);
    setJSONFieldIfNotDefault(j, "Key", l.key);
    setJSONFieldIfNotDefault(j, "MaxDevices", l.maxDevices);
    setJSONFieldIfNotDefault(j, "DeviceRegistrations", l.registrationCount);
    setJSONFieldIfNotDefault(j, "Created", l.createdEpochSeconds);
}

void dfx::api::from_json(const nlohmann::json& j, License& l)
{
    getValidField(j, "ID", l.id);
    getValidField(j, "LicenseType", l.licenseType);
    getValidField(j, "StatusID", l.status);
    getValidField(j, "Expiration", l.expiration);
    getValidField(j, "Key", l.key);
    getValidField(j, "MaxDevices", l.maxDevices);
    getValidField(j, "DeviceRegistrations", l.registrationCount);
    getValidField(j, "Created", l.createdEpochSeconds);
}
