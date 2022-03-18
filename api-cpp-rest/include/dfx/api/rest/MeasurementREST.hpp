// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_MEASUREMENT_REST_H
#define DFX_API_CLOUD_MEASUREMENT_REST_H

#include "dfx/api/MeasurementAPI.hpp"

#include <memory>

namespace dfx::api::rest
{

class CloudREST;

class MeasurementREST : public MeasurementAPI
{
public:
    MeasurementREST() = default;

    ~MeasurementREST() override = default;

    CloudStatus list(const CloudConfig& config,
                     const std::unordered_map<MeasurementFilter, std::string>& filters,
                     uint16_t offset,
                     std::vector<Measurement>& measurements,
                     int16_t& totalCount) override;

    CloudStatus retrieve(const CloudConfig& config,
                         const std::string& measurementID,
                         Measurement& measurementData) override;
};

} // namespace dfx::api::rest

#endif // DFX_API_CLOUD_MEASUREMENT_REST_H
