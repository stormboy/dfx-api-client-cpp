// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/types/StudyTypes.hpp"
#include "dfx/api/CloudLog.hpp"
#include "dfx/api/CloudTypes.hpp"

#include "fmt/format.h"

using namespace dfx::api;

const std::map<StudyStatus, std::string> StudyStatusMapper::toString{{StudyStatus::UNKNOWN, "UNKNOWN"},
                                                                     {StudyStatus::ACTIVE, "ACTIVE"},
                                                                     {StudyStatus::INACTIVE, "INACTIVE"},
                                                                     {StudyStatus::DELETED, "DELETED"}};

const std::map<std::string, StudyStatus> StudyStatusMapper::toEnum{{"UNKNOWN", StudyStatus::UNKNOWN},
                                                                   {"ACTIVE", StudyStatus::ACTIVE},
                                                                   {"INACTIVE", StudyStatus::INACTIVE},
                                                                   {"DELETED", StudyStatus::DELETED}};

StudyStatus StudyStatusMapper::getEnum(const std::string& status)
{
    auto it = StudyStatusMapper::toEnum.find(status);
    if (it != StudyStatusMapper::toEnum.end()) {
        return it->second;
    } else {
        return StudyStatus::UNKNOWN;
    }
}

std::string StudyStatusMapper::getString(StudyStatus status)
{
    auto it = StudyStatusMapper::toString.find(status);
    if (it != StudyStatusMapper::toString.end()) {
        return it->second;
    } else {
        return "UNKNOWN";
    }
}

void dfx::api::to_json(nlohmann::json& j, const Study& s)
{
    j = nlohmann::json();
    setJSONFieldIfNotDefault(j, "ID", s.id);
    setJSONFieldIfNotDefault(j, "Name", s.name);
    setJSONFieldIfNotDefault(j, "Description", s.description);
    setJSONFieldIfNotDefault(j, "StudyTemplateID", s.templateID);
    setJSONFieldIfNotDefault(j, "Participants", s.numberParticipants);
    setJSONFieldIfNotDefault(j, "Measurements", s.numberMeasurements);
    setJSONFieldIfNotDefault(j, "Created", s.createdEpochSeconds);
    setJSONFieldIfNotDefault(j, "Updated", s.updatedEpochSeconds);

    if (s.status != StudyStatus::UNKNOWN) {
        j["StatusID"] = StudyStatusMapper::getString(s.status);
    }
}

void dfx::api::from_json(const nlohmann::json& j, Study& s)
{
    getValidField(j, "ID", s.id);
    getValidField(j, "Name", s.name);
    getValidField(j, "Description", s.description);
    getValidField(j, "StudyTemplateID", s.templateID);
    getValidField(j, "Participants", s.numberParticipants);
    getValidField(j, "Measurements", s.numberMeasurements);
    getValidField(j, "Created", s.createdEpochSeconds);
    getValidField(j, "Updated", s.updatedEpochSeconds);

    nlohmann::json field;
    if (jsonValidField(j, "StatusID", field)) {
        const std::string& statusType = field;
        s.status = StudyStatusMapper::getEnum(statusType);
    } else {
        s.status = StudyStatus::UNKNOWN;
    }
}