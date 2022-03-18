// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_STUDY_TYPES_H
#define DFX_API_CLOUD_STUDY_TYPES_H

#include "dfx/api/CloudAPI_Export.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dfx::api
{

enum class StudyStatus : std::uint8_t
{
    UNKNOWN = 0,
    ACTIVE = 1,
    INACTIVE = 2,
    DELETED = 3
};

using Study = struct Study
{
    std::string id;
    std::string name;
    std::string description;
    std::string templateID;
    StudyStatus status;
    uint64_t createdEpochSeconds;
    uint64_t updatedEpochSeconds;

    uint64_t numberMeasurements; // REST/WEBSOCKET
    uint64_t numberParticipants; // REST/WEBSOCKET
};

using StudyType = struct StudyType
{
    std::string id;
    std::string name;
    std::string description;
    StudyStatus status;
};

using StudyTemplate = struct StudyTemplate
{
    std::string id;
    std::string name;
    std::string description;
    StudyStatus status;
    std::string bundleID;
    std::string config;
    std::string studyTypeID;
    std::vector<std::string> signalIDS;
    uint64_t createdEpochSeconds;
};

struct StudyStatusMapper
{
    static const std::map<StudyStatus, std::string> toString;
    static const std::map<std::string, StudyStatus> toEnum;

    DFXCLOUD_EXPORT static StudyStatus getEnum(const std::string& status);
    DFXCLOUD_EXPORT static std::string getString(StudyStatus status);
};

DFXCLOUD_EXPORT void to_json(nlohmann::json& j, const Study& s);
DFXCLOUD_EXPORT void from_json(const nlohmann::json& j, Study& s);

} // namespace dfx::api

DFXCLOUD_EXPORT std::ostream& operator<<(std::ostream& os, const dfx::api::Study& study);

#endif // DFX_API_CLOUD_STUDY_TYPES_H
