// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_SIGNAL_TYPES_H
#define DFX_API_CLOUD_SIGNAL_TYPES_H

#include "dfx/api/CloudAPI_Export.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dfx::api
{
enum class SignalCategory : std::uint8_t
{
    UNKNOWN = 0,
    MODEL = 1,
    ALGORITHM = 2,
    CLASSIFIER = 3,
    SIGNAL = 4,
    SOURCE = 5
};

using Signal = struct SignalDetail
{
    std::string id;
    std::string name;
    std::string version;
    std::string description;
    std::string unit;
    SignalCategory category;
    double modelMinAmplitude;
    double modelMaxAmplitude;
    double humanMinAmplitude;
    double humanMaxAmplitude;
    uint32_t modelMinSeconds;
    uint32_t initialDelaySeconds;
};

struct SignalCategoryMapper
{
    static const std::map<SignalCategory, std::string> toString;
    static const std::map<std::string, SignalCategory> toEnum;

    DFXCLOUD_EXPORT static SignalCategory getEnum(const std::string& status);
    DFXCLOUD_EXPORT static std::string getString(SignalCategory status);
};

DFXCLOUD_EXPORT void to_json(nlohmann::json& j, const Signal& s);
DFXCLOUD_EXPORT void from_json(const nlohmann::json& j, Signal& s);

} // namespace dfx::api

#endif // DFX_API_CLOUD_SIGNAL_TYPES_H
