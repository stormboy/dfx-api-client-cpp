// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/MeasurementAPI.hpp"

using namespace dfx::api;

CloudStatus MeasurementAPI::list(const CloudConfig& config,
                                 const std::unordered_map<MeasurementFilter, std::string>& filters,
                                 uint16_t offset,
                                 std::vector<Measurement>& measurements,
                                 int16_t& totalCount)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus MeasurementAPI::retrieve(const CloudConfig& config,
                                     const std::string& measurementID,
                                     Measurement& measurementData)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

// Polyfill for web based on retrieve, gRPC has it natively implemented
CloudStatus MeasurementAPI::retrieveMultiple(const CloudConfig& config,
                                             const std::vector<std::string>& measurementIDs,
                                             std::vector<Measurement>& measurements)
{
    // Validate will occur by first retrieve call

    std::vector<Measurement> measurementList;
    for (const auto& id : measurementIDs) {
        Measurement measurement;
        auto status = retrieve(config, id, measurement);
        if (status.OK()) {
            measurementList.push_back(measurement);
        } else {
            return status;
        }
    }

    // Copy all the items we retrieved - this ensures measurements state consistent on failure
    // and allows client to pass existing items in list without us clearing.
    measurements.insert(measurements.end(), measurementList.begin(), measurementList.end());
    return CloudStatus(CLOUD_OK);
}
