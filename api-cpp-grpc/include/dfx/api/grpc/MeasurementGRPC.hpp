// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_MEASUREMENT_GRPC_H
#define DFX_API_CLOUD_MEASUREMENT_GRPC_H

#include "dfx/api/MeasurementAPI.hpp"
#include "dfx/measurements/v2/measurements.grpc.pb.h"

namespace dfx::api::grpc
{

class CloudGRPC;

class MeasurementGRPC : public MeasurementAPI
{
public:
    MeasurementGRPC(const CloudConfig& config, const std::shared_ptr<CloudGRPC>& cloudGRPC);

    ~MeasurementGRPC() override = default;

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

private:
    std::unique_ptr<dfx::measurements::v2::API::Stub> grpcMeasurementsStub;
};

} // namespace dfx::api::grpc

#endif // DFX_API_CLOUD_MEASUREMENT_GRPC_H
