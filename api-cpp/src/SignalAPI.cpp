// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/SignalAPI.hpp"

using namespace dfx::api;

CloudStatus SignalAPI::list(const CloudConfig& config,
                            const std::unordered_map<SignalFilter, std::string>& filters,
                            uint16_t offset,
                            std::vector<Signal>& signals,
                            int16_t& totalCount)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus SignalAPI::retrieve(const CloudConfig& config, const std::string& signalID, Signal& signal)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

// Polyfill for web based on retrieve, gRPC it is natively implemented
CloudStatus SignalAPI::retrieveMultiple(const CloudConfig& config,
                                        const std::vector<std::string>& signalIDs,
                                        std::vector<Signal>& signals)
{
    // Validate will occur by first retrieve call

    std::vector<Signal> signalList;
    for (const auto& signalID : signalIDs) {
        Signal signal;
        auto status = retrieve(config, signalID, signal);
        if (status.OK()) {
            signalList.push_back(signal);
        } else {
            return status;
        }
    }

    // Copy all the items we retrieved - this ensures org state consistent on failure
    // and allows client to pass existing items in list without us clearing.
    signals.insert(signals.end(), signalList.begin(), signalList.end());
    return CloudStatus(CLOUD_OK);
}

CloudStatus SignalAPI::retrieveStudySignalIDs(const CloudConfig& config,
                                              const std::string& studyID,
                                              std::vector<std::string>& signalIDs)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus SignalAPI::retrieveSignalDetail(const CloudConfig& config,
                                            const std::string& signalID,
                                            Signal& signalDetail)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus SignalAPI::retrieveSignalDetails(const CloudConfig& config,
                                             const std::list<std::string>& signalIDs,
                                             std::vector<Signal>& signalDetails)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}