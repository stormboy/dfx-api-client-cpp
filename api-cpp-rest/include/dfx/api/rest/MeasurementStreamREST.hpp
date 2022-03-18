// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_MEASUREMENT_STREAM_REST_H
#define DFX_API_CLOUD_MEASUREMENT_STREAM_REST_H

#include "dfx/api/MeasurementStreamAPI.hpp"

namespace dfx::api::rest
{
class MeasurementStreamREST : public MeasurementStreamAPI
{
public:
    MeasurementStreamREST(const CloudConfig& config);

    ~MeasurementStreamREST() override;

private:
    CloudStatus setupStream(const CloudConfig& config, const std::string& studyID) override;
};

} // namespace dfx::api::rest

#endif // DFX_API_CLOUD_MEASUREMENT_STREAM_REST_H
