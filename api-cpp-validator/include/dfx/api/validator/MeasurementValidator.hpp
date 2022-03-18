// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_MEASUREMENT_VALIDATOR_H
#define DFX_API_CLOUD_MEASUREMENT_VALIDATOR_H

#include "dfx/api/MeasurementAPI.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

namespace dfx::api::validator
{

class MeasurementValidator : public MeasurementAPI
{
public:
    MeasurementValidator() = default;

    ~MeasurementValidator() override = default;

    CloudStatus list(const CloudConfig& config,
                     const std::unordered_map<MeasurementFilter, std::string>& filters,
                     uint16_t offset,
                     std::vector<Measurement>& measurements,
                     int16_t& totalCount) override;

    CloudStatus retrieve(const CloudConfig& config,
                         const std::string& measurementID,
                         Measurement& measurementData) override;

    CloudStatus retrieveMultiple(const CloudConfig& config,
                                 const std::vector<std::string>& measurementIDs,
                                 std::vector<Measurement>& measurements) override;

    static const MeasurementValidator& instance();
};

} // namespace dfx::api::validator

#endif // DFX_API_CLOUD_MEASUREMENT_VALIDATOR_H
