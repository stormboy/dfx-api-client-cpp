// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/types/DeviceTypes.hpp"
#include "dfx/api/CloudLog.hpp"
#include "dfx/api/CloudTypes.hpp"

using namespace dfx::api;

// NOLINTNEXTLINE(cert-err58-cpp)
const std::map<DeviceType, std::string> DeviceTypeMapper::toString = {{DeviceType::UNKNOWN, "UNKNOWN"},
                                                                      {DeviceType::IPHONE, "IPHONE"},
                                                                      {DeviceType::ANDROID_PHONE, "ANDROID_PHONE"},
                                                                      //        {DeviceTypes::WIN32,          "WIN32"},
                                                                      {DeviceType::DARWIN, "DARWIN"},
                                                                      {DeviceType::IPAD, "IPAD"},
                                                                      {DeviceType::WINDOWS_TABLET, "WINDOWS_TABLET"},
                                                                      {DeviceType::LINUX, "LINUX"},
                                                                      {DeviceType::DEBIAN, "DEBIAN"}};

// NOLINTNEXTLINE(cert-err58-cpp)
const std::map<std::string, DeviceType> DeviceTypeMapper::toEnum{{"UNKNOWN", DeviceType::UNKNOWN},
                                                                 {"IPHONE", DeviceType::IPHONE},
                                                                 {"ANDROID_PHONE", DeviceType::ANDROID_PHONE},
                                                                 //       {"WIN32",          DeviceTypes::WIN32},
                                                                 {"DARWIN", DeviceType::DARWIN},
                                                                 {"IPAD", DeviceType::IPAD},
                                                                 {"WINDOWS_TABLET", DeviceType::WINDOWS_TABLET},
                                                                 {"LINUX", DeviceType::LINUX},
                                                                 {"DEBIAN", DeviceType::DEBIAN}};

DeviceType DeviceTypeMapper::getEnum(const std::string& type)
{
    try {
        return DeviceTypeMapper::toEnum.at(type);
    } catch (const std::out_of_range&) {
        cloudLog(CLOUD_LOG_LEVEL_WARNING, "Unexpected DeviceType: %s\n", type.c_str());
        return DeviceType::UNKNOWN;
    }
}

std::string DeviceTypeMapper::getString(DeviceType type)
{
    try {
        return DeviceTypeMapper::toString.at(type);
    } catch (const std::out_of_range&) {
        cloudLog(CLOUD_LOG_LEVEL_WARNING, "Unexpected DeviceType: %d\n", static_cast<uint16_t>(type));
        return "UNKNOWN";
    }
}

// NOLINTNEXTLINE(cert-err58-cpp)
const std::map<DeviceStatus, std::string> DeviceStatusMapper::toString{
    {DeviceStatus::UNKNOWN, "UNKNOWN"},
    {DeviceStatus::ACTIVE, "ACTIVE"},
    {DeviceStatus::INACTIVE, "INACTIVE"},
    {DeviceStatus::DELETED, "DELETED"},
    {DeviceStatus::DECOMMISSIONED, "DECOMMISSIONED"}};

// NOLINTNEXTLINE(cert-err58-cpp)
const std::map<std::string, DeviceStatus> DeviceStatusMapper::toEnum{{"UNKNOWN", DeviceStatus::UNKNOWN},
                                                                     {"ACTIVE", DeviceStatus::ACTIVE},
                                                                     {"INACTIVE", DeviceStatus::INACTIVE},
                                                                     {"DELETED", DeviceStatus::DELETED},
                                                                     {"DECOMMISSIONED", DeviceStatus::DECOMMISSIONED}};

DeviceStatus DeviceStatusMapper::getEnum(const std::string& status)
{
    try {
        return DeviceStatusMapper::toEnum.at(status);
    } catch (const std::out_of_range&) {
        cloudLog(CLOUD_LOG_LEVEL_WARNING, "Unexpected DeviceStatus: %s\n", status.c_str());
        return DeviceStatus::UNKNOWN;
    }
}

std::string DeviceStatusMapper::getString(DeviceStatus status)
{
    try {
        return DeviceStatusMapper::toString.at(status);
    } catch (const std::out_of_range&) {
        cloudLog(CLOUD_LOG_LEVEL_WARNING, "Unexpected DeviceStatus: %d\n", static_cast<uint16_t>(status));
        return "UNKNOWN";
    }
}

void dfx::api::to_json(nlohmann::json& j, const Device& d)
{
    j = nlohmann::json();
    setJSONFieldIfNotDefault(j, "ID", d.id);
    setJSONFieldIfNotDefault(j, "Name", d.name);
    setJSONFieldIfNotDefault(j, "Identifier", d.identifier);
    setJSONFieldIfNotDefault(j, "Version", d.version);
    setJSONFieldIfNotDefault(j, "Created", d.createdEpochSeconds);
    setJSONFieldIfNotDefault(j, "Updated", d.updatedEpochSeconds);
    setJSONFieldIfNotDefault(j, "MeasurementCount", d.numberMeasurements);
    setJSONFieldIfNotDefault(j, "Region", d.region);

    if (d.type != DeviceType::UNKNOWN) {
        j["DeviceTypeID"] = DeviceTypeMapper::getString(d.type);
    }
    if (d.status != DeviceStatus::UNKNOWN) {
        j["StatusID"] = DeviceStatusMapper::getString(d.status);
    }
}

void dfx::api::from_json(const nlohmann::json& j, Device& d)
{
    getValidField(j, "ID", d.id);
    getValidField(j, "Name", d.name);

    nlohmann::json field;
    if (jsonValidField(j, "DeviceTypeID", field)) {
        const std::string& deviceType = field;
        d.type = DeviceTypeMapper::getEnum(deviceType);
    } else {
        d.type = DeviceType::UNKNOWN;
    }

    if (jsonValidField(j, "StatusID", field)) {
        const std::string& statusID = field;
        d.status = DeviceStatusMapper::getEnum(statusID);
    } else {
        d.status = DeviceStatus::UNKNOWN;
    }

    getValidField(j, "Identifier", d.identifier);
    getValidField(j, "Version", d.version);
    getValidField(j, "Created", d.createdEpochSeconds);
    getValidField(j, "Updated", d.updatedEpochSeconds);
    getValidField(j, "MeasurementCount", d.numberMeasurements);
    getValidField(j, "Region", d.region);
}