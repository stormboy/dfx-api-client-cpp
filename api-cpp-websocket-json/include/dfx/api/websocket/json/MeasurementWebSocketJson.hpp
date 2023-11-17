// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_MEASUREMENT_WEBSOCKET_JSON_H
#define DFX_API_CLOUD_MEASUREMENT_WEBSOCKET_JSON_H

#include "dfx/api/MeasurementAPI.hpp"

#include <memory>

namespace dfx::api::websocket::json
{

class CloudWebSocketJson;

class MeasurementWebSocketJson : public MeasurementAPI
{
public:
    MeasurementWebSocketJson(const CloudConfig& config, std::shared_ptr<CloudWebSocketJson> cloudWebSocketJson);

    ~MeasurementWebSocketJson() override = default;

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
    std::shared_ptr<CloudWebSocketJson> cloudWebSocketJson;
};

} // namespace dfx::api::websocket::json

#endif // DFX_API_CLOUD_MEASUREMENT_WEBSOCKET_JSON_H
