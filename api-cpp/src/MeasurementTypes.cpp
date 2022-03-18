// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/types/MeasurementTypes.hpp"
#include "dfx/api/CloudTypes.hpp"

using namespace dfx::api;

const std::map<MeasurementStatus, std::string> MeasurementStatusMapper::toString{
    {MeasurementStatus::UNKNOWN, "UNKNOWN"},
    {MeasurementStatus::IN_PROGRESS, "IN_PROGRESS"},
    {MeasurementStatus::CANCELLED, "CANCELLED"},
    // {MeasurementStatus::STATUS_ERROR, "STATUS_ERROR"},
    {MeasurementStatus::COMPLETE, "COMPLETE"},
    {MeasurementStatus::WARNING, "WARNING"},
    {MeasurementStatus::ERROR_STATUS, "ERROR"}};

const std::map<std::string, MeasurementStatus> MeasurementStatusMapper::toEnum{
    {"UNKNOWN", MeasurementStatus::UNKNOWN},
    {"IN_PROGRESS", MeasurementStatus::IN_PROGRESS},
    {"CANCELLED", MeasurementStatus::CANCELLED},
    // {"STATUS_ERROR", MeasurementStatus::STATUS_ERROR},
    {"COMPLETE", MeasurementStatus::COMPLETE},
    {"WARNING", MeasurementStatus::WARNING},
    {"ERROR", MeasurementStatus::ERROR_STATUS}};

MeasurementStatus MeasurementStatusMapper::getEnum(const std::string& status)
{
    try {
        return MeasurementStatusMapper::toEnum.at(status);
    } catch (const std::out_of_range&) {
        return MeasurementStatus::UNKNOWN;
    }
}

std::string MeasurementStatusMapper::getString(MeasurementStatus status)
{
    try {
        return MeasurementStatusMapper::toString.at(status);
    } catch (const std::out_of_range&) {
        return "UNKNOWN";
    }
}

void dfx::api::to_json(nlohmann::json& j, const Measurement& m)
{
    j = nlohmann::json();
    setJSONFieldIfNotDefault(j, "ID", m.id);
    setJSONFieldIfNotDefault(j, "StudyID", m.studyID);
    setJSONFieldIfNotDefault(j, "DeviceID", m.deviceID);
    setJSONFieldIfNotDefault(j, "UserProfileID", m.userProfileID);
    setJSONFieldIfNotDefault(j, "UserProfileName", m.userProfileName);
    setJSONFieldIfNotDefault(j, "Created", m.createdEpochSeconds);
    setJSONFieldIfNotDefault(j, "Updated", m.updatedEpochSeconds);
    if (m.status != MeasurementStatus::UNKNOWN) {
        j["Status"] = MeasurementStatusMapper::getString(m.status);
    }
    setJSONFieldIfNotDefault(j, "OrganizationID", m.organizationID);
    setJSONFieldIfNotDefault(j, "StatusID", m.statusID);
    setJSONFieldIfNotDefault(j, "UserID", m.userID);
    setJSONFieldIfNotDefault(j, "Resolution", m.resolution);
    setJSONFieldIfNotDefault(j, "DeviceVersion", m.deviceVersion);
    setJSONFieldIfNotDefault(j, "Mode", m.mode);
    setJSONFieldIfNotDefault(j, "City", m.city);
    setJSONFieldIfNotDefault(j, "State", m.state);
    setJSONFieldIfNotDefault(j, "Country", m.country);
    setJSONFieldIfNotDefault(j, "Region", m.region);
    setJSONFieldIfNotDefault(j, "PartnerID", m.partnerID);
    setJSONFieldIfNotDefault(j, "DataSizeBytes", m.dataSizeBytes);

    // These are not really strings, they are objects which we store in a string
    // "type must be string, but is object"
    if (!m.comments.empty()) {
        j["Comments"] = nlohmann::json::parse(m.comments);
    }
    if (!m.results.empty()) {
        j["Results"] = nlohmann::json::parse(m.results);
    }
    if (!m.files.empty()) {
        j["Files"] = nlohmann::json::parse(m.files);
    }
    if (!m.signalNames.empty()) {
        j["SignalNames"] = nlohmann::json::parse(m.signalNames);
    }
    if (!m.signalDescriptions.empty()) {
        j["SignalDescriptions"] = nlohmann::json::parse(m.signalDescriptions);
    }
    if (!m.signalConfig.empty()) {
        j["SignalConfig"] = nlohmann::json::parse(m.signalConfig);
    }
    if (!m.signalUnits.empty()) {
        j["SignalUnits"] = nlohmann::json::parse(m.signalUnits);
    }
}

void dfx::api::from_json(const nlohmann::json& j, Measurement& m)
{
    getValidField(j, "ID", m.id);
    getValidField(j, "StudyID", m.studyID);
    getValidField(j, "DeviceID", m.deviceID);
    getValidField(j, "UserProfileID", m.userProfileID);
    getValidField(j, "UserProfileName", m.userProfileName);
    getValidField(j, "Created", m.createdEpochSeconds);
    getValidField(j, "Updated", m.updatedEpochSeconds);

    nlohmann::json field;
    if (jsonValidField(j, "Status", field)) {
        const std::string& status = field;
        m.status = MeasurementStatusMapper::getEnum(status);
    } else {
        m.status = MeasurementStatus::UNKNOWN;
    }

    getValidField(j, "OrganizationID", m.organizationID);
    getValidField(j, "StatusID", m.statusID);
    getValidField(j, "UserID", m.userID);
    getValidField(j, "Resolution", m.resolution);
    getValidField(j, "DeviceVersion", m.deviceVersion);
    getValidField(j, "Mode", m.mode);
    getValidField(j, "City", m.city);
    getValidField(j, "State", m.state);
    getValidField(j, "Country", m.country);
    getValidField(j, "Region", m.region);
    getValidField(j, "PartnerID", m.partnerID);
    getValidField(j, "DataSizeBytes", m.dataSizeBytes);

    // These are not really strings, they are objects which we store in a string
    // "type must be string, but is object"
    getStringifedField(j, "Comments", m.comments);
    getStringifedField(j, "Results", m.results);
    getStringifedField(j, "Files", m.files);
    getStringifedField(j, "SignalNames", m.signalNames);
    getStringifedField(j, "SignalDescriptions", m.signalDescriptions);
    getStringifedField(j, "SignalConfig", m.signalConfig);
    getStringifedField(j, "SignalUnits", m.signalUnits);
}
