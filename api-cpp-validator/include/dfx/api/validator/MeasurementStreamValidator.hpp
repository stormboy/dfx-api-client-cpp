// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_MEASUREMENT_STREAM_VALIDATOR_H
#define DFX_API_CLOUD_MEASUREMENT_STREAM_VALIDATOR_H

#include "dfx/api/MeasurementStreamAPI.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

namespace dfx::api::validator
{

class MeasurementStreamValidator : public MeasurementStreamAPI
{
public:
    MeasurementStreamValidator() = default;

    ~MeasurementStreamValidator() override;

    virtual CloudStatus setupStream(const CloudConfig& config, const std::string& studyID) override;

    virtual CloudStatus sendChunk(const CloudConfig& config,
                                  const std::vector<uint8_t>& chunk,
                                  bool isLastChunk) override;

    static const MeasurementStreamValidator& instance();
};

} // namespace dfx::api::validator

#endif // DFX_API_CLOUD_MEASUREMENT_STREAM_VALIDATOR_H
