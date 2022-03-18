// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_CLOUD_VALIDATOR_API_H
#define DFX_CLOUD_VALIDATOR_API_H

#ifndef WITH_VALIDATORS

#define DFX_CLOUD_VALIDATOR_MACRO(type, check) ;

#else

#include "dfx/api/CloudAPI.hpp"
#include "dfx/api/CloudConfig.hpp"

#include <list>
#include <memory>
#include <string>

#include "dfx/api/validator/DeviceValidator.hpp"
#include "dfx/api/validator/LicenseValidator.hpp"
#include "dfx/api/validator/MeasurementStreamValidator.hpp"
#include "dfx/api/validator/MeasurementValidator.hpp"
#include "dfx/api/validator/OrganizationValidator.hpp"
#include "dfx/api/validator/ProfileValidator.hpp"
#include "dfx/api/validator/SignalValidator.hpp"
#include "dfx/api/validator/StudyValidator.hpp"
#include "dfx/api/validator/UserValidator.hpp"

#define DFX_CLOUD_VALIDATOR_MACRO(type, check)                                                                         \
    {                                                                                                                  \
        const auto& checkedStatus =                                                                                    \
            const_cast<dfx::api::validator::type*>(&dfx::api::validator::type::instance())->check;                     \
        if (!checkedStatus.OK()) {                                                                                     \
            return checkedStatus;                                                                                      \
        }                                                                                                              \
    }

namespace dfx::api::validator
{

class CloudValidator : public CloudAPI
{
public:
    CloudValidator();

    ~CloudValidator() override = default;

    CloudStatus connect(const CloudConfig& config) override;

    CloudStatus getServerStatus(CloudConfig& config) override;

    CloudStatus login(CloudConfig& config) override;

    CloudStatus logout(CloudConfig& config) override;

    CloudStatus registerDevice(CloudConfig& config, const std::string& appName, const std::string& appVersion) override;

    CloudStatus unregisterDevice(CloudConfig& config) override;

    CloudStatus validateToken(const CloudConfig& config, const std::string& userToken) override;

    CloudStatus switchEffectiveOrganization(CloudConfig& config, const std::string& organizationID) override;

    static const CloudValidator& instance();
};

} // namespace dfx::api::validator
#endif // WITH_VALIDATORS
#endif // DFX_CLOUD_VALIDATOR_API_H
