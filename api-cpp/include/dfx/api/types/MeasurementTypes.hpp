// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_MEASUREMENT_TYPES_H
#define DFX_API_CLOUD_MEASUREMENT_TYPES_H

#include "dfx/api/CloudAPI_Export.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dfx::api
{
enum class MeasurementStatus : std::int8_t
{
    UNKNOWN = 0,
    IN_PROGRESS = 1,
    CANCELLED = 2,
    ERROR_STATUS = 3, // Windows ERROR is a reserved keyword
    COMPLETE = 4,
    WARNING = 5
};

using Measurement = struct Measurement
{
    std::string id;
    std::string studyID;
    std::string deviceID;
    uint64_t createdEpochSeconds;
    uint64_t updatedEpochSeconds;
    MeasurementStatus status;
    std::string organizationID;
    std::string statusID;
    std::string userID;
    std::string userProfileID;   // Web
    std::string userProfileName; // Web
    uint16_t resolution;
    std::string deviceVersion;
    std::string comments;
    std::string mode;
    std::string city;
    std::string state;
    std::string country;
    std::string region;
    std::string partnerID;
    std::string signalNames;
    std::string signalDescriptions;
    std::string signalConfig;
    std::string signalUnits;
    uint64_t dataSizeBytes;

    std::string files;   // Web JSON string
    std::string results; // Web JSON string
};

struct MeasurementStatusMapper
{
    static const std::map<MeasurementStatus, std::string> toString;
    static const std::map<std::string, MeasurementStatus> toEnum;

    DFXCLOUD_EXPORT static MeasurementStatus getEnum(const std::string& status);
    DFXCLOUD_EXPORT static std::string getString(MeasurementStatus status);
};

DFXCLOUD_EXPORT void to_json(nlohmann::json& j, const Measurement& m);
DFXCLOUD_EXPORT void from_json(const nlohmann::json& j, Measurement& m);

} // namespace dfx::api

#endif // DFX_API_CLOUD_MEASUREMENT_TYPES_H
