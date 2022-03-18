// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_SIGNAL_REST_H
#define DFX_API_CLOUD_SIGNAL_REST_H

#include "dfx/api/SignalAPI.hpp"

#include <memory>

namespace dfx::api::rest
{

class CloudREST;

class SignalREST : public SignalAPI
{
public:
    SignalREST() = default;

    ~SignalREST() override = default;

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
};

} // namespace dfx::api::rest

#endif // DFX_API_CLOUD_SIGNAL_REST_H
