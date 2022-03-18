// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_LICENSE_VALIDATOR_H
#define DFX_API_CLOUD_LICENSE_VALIDATOR_H

#include "dfx/api/LicenseAPI.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

namespace dfx::api::validator
{

class CloudValidator;

class LicenseValidator : public LicenseAPI
{
public:
    LicenseValidator() = default;

    ~LicenseValidator() = default;

    CloudStatus list(const CloudConfig& config,
                     const std::unordered_map<LicenseFilter, std::string>& filters,
                     uint16_t offset,
                     std::vector<License>& licenses,
                     int16_t& totalCount) override;

    static const LicenseValidator& instance();
};

} // namespace dfx::api::validator

#endif // DFX_API_CLOUD_LICENSE_VALIDATOR_H
