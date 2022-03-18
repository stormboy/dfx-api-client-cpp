// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/LicenseAPI.hpp"

using namespace dfx::api;

CloudStatus LicenseAPI::list(const CloudConfig& config,
                             const std::unordered_map<LicenseFilter, std::string>& filters,
                             uint16_t offset,
                             std::vector<License>& licenses,
                             int16_t& totalCount)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}