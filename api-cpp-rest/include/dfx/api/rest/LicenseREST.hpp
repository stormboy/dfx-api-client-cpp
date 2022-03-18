// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_LICENSE_REST_H
#define DFX_API_CLOUD_LICENSE_REST_H

#include "dfx/api/LicenseAPI.hpp"

namespace dfx::api::rest
{

class CloudREST;

class LicenseREST : public LicenseAPI
{
public:
    LicenseREST() = default;

    ~LicenseREST() = default;

    CloudStatus list(const CloudConfig& config,
                     const std::unordered_map<LicenseFilter, std::string>& filters,
                     uint16_t offset,
                     std::vector<License>& licenses,
                     int16_t& totalCount) override;
};

} // namespace dfx::api::rest

#endif // DFX_API_CLOUD_LICENSE_REST_H
