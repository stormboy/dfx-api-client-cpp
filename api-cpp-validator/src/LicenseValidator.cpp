// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/validator/LicenseValidator.hpp"
#include "CloudValidatorMacros.hpp"

using namespace dfx::api;
using namespace dfx::api::validator;

CloudStatus LicenseValidator::list(const CloudConfig& config,
                                   const std::unordered_map<LicenseFilter, std::string>& filters,
                                   uint16_t offset,
                                   std::vector<License>& licenses,
                                   int16_t& totalCount)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    return CloudStatus(CLOUD_OK);
}

const LicenseValidator& LicenseValidator::instance()
{
    static const LicenseValidator instance;
    return instance;
}
