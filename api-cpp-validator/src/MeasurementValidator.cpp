// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/validator/MeasurementValidator.hpp"

#include "CloudValidatorMacros.hpp"

using namespace dfx::api;
using namespace dfx::api::validator;

const MeasurementValidator& MeasurementValidator::instance()
{
    static const MeasurementValidator instance;
    return instance;
}

CloudStatus MeasurementValidator::list(const CloudConfig& config,
                                       const std::unordered_map<MeasurementFilter, std::string>& filters,
                                       uint16_t offset,
                                       std::vector<Measurement>& measurements,
                                       int16_t& totalCount)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    return CloudStatus(CLOUD_OK);
}

CloudStatus MeasurementValidator::retrieve(const CloudConfig& config,
                                           const std::string& measurementID,
                                           Measurement& measurementData)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    return CloudStatus(CLOUD_OK);
}

CloudStatus MeasurementValidator::retrieveMultiple(const CloudConfig& config,
                                                   const std::vector<std::string>& measurementIDs,
                                                   std::vector<Measurement>& measurements)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(measurementIDs);
    return CloudStatus(CLOUD_OK);
}