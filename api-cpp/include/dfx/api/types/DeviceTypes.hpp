// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_DEVICE_TYPES_H
#define DFX_API_CLOUD_DEVICE_TYPES_H

#include "dfx/api/CloudAPI_Export.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dfx::api
{

enum class DeviceType : std::int8_t
{
    UNKNOWN = 0,
    IPHONE,
    ANDROID_PHONE,
    // WIN32,
    DARWIN,
    IPAD,
    WINDOWS_TABLET,
    // LINUX,
    DEBIAN
};

enum class DeviceStatus : std::int8_t
{
    UNKNOWN = 0,
    ACTIVE = 1,
    INACTIVE = 2,
    DELETED = 3,
    DECOMMISSIONED = 4 // WEB ONLY
};

using Device = struct Device
{
    std::string id;
    std::string name;
    DeviceType type;
    DeviceStatus status;
    std::string identifier;
    std::string version;
    uint64_t createdEpochSeconds;
    uint64_t updatedEpochSeconds;

    uint64_t numberMeasurements; // WEB ONLY
    std::string region;          // WEB ONLY
};

struct DeviceTypeMapper
{
    static const std::map<DeviceType, std::string> toString;
    static const std::map<std::string, DeviceType> toEnum;

    DFXCLOUD_EXPORT static DeviceType getEnum(const std::string& type);

    DFXCLOUD_EXPORT static std::string getString(DeviceType type);
};

struct DeviceStatusMapper
{
    static const std::map<DeviceStatus, std::string> toString;
    static const std::map<std::string, DeviceStatus> toEnum;

    DFXCLOUD_EXPORT static DeviceStatus getEnum(const std::string& status);

    DFXCLOUD_EXPORT static std::string getString(DeviceStatus status);
};

DFXCLOUD_EXPORT void to_json(nlohmann::json& j, const Device& d);
DFXCLOUD_EXPORT void from_json(const nlohmann::json& j, Device& d);

} // namespace dfx::api

#endif // DFX_API_CLOUD_DEVICE_TYPES_H
