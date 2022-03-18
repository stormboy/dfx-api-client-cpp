// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_SIGNAL_GRPC_H
#define DFX_API_CLOUD_SIGNAL_GRPC_H

#include "dfx/api/SignalAPI.hpp"

#include "dfx/signals/v2/signals.grpc.pb.h"
#include "dfx/studysignals/v2/studysignals.grpc.pb.h"

namespace dfx::api::grpc
{

class CloudGRPC;

class SignalGRPC : public SignalAPI
{
public:
    SignalGRPC(const CloudConfig& config, const std::shared_ptr<CloudGRPC>& cloudGRPC);

    ~SignalGRPC() override = default;

    CloudStatus list(const CloudConfig& config,
                     const std::unordered_map<SignalFilter, std::string>& filters,
                     uint16_t offset,
                     std::vector<Signal>& signals,
                     int16_t& totalCount) override;

    CloudStatus retrieve(const CloudConfig& config, const std::string& signalID, Signal& signal) override;

    CloudStatus retrieveMultiple(const CloudConfig& config,
                                 const std::vector<std::string>& signalIDs,
                                 std::vector<Signal>& signals) override;

    CloudStatus retrieveStudySignalIDs(const CloudConfig& config,
                                       const std::string& studyID,
                                       std::vector<std::string>& signalIDs) override;

    CloudStatus retrieveSignalDetail(const CloudConfig& config,
                                     const std::string& signalID,
                                     Signal& signalDetail) override;

    CloudStatus retrieveSignalDetails(const CloudConfig& config,
                                      const std::list<std::string>& signalIDs,
                                      std::vector<Signal>& signalDetails) override;

private:
    std::unique_ptr<dfx::studysignals::v2::API::Stub> grpcStudySignalsStub;
    std::unique_ptr<dfx::signals::v2::API::Stub> grpcSignalsStub;
};

} // namespace dfx::api::grpc

#endif // DFX_API_CLOUD_SIGNAL_GRPC_H
