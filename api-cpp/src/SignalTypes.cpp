// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/types/SignalTypes.hpp"
#include "dfx/api/CloudTypes.hpp"

using namespace dfx::api;

// NOLINTNEXTLINE(cert-err58-cpp)
const std::map<SignalCategory, std::string> SignalCategoryMapper::toString = {
    {SignalCategory::UNKNOWN, "UNKNOWN"},
    {SignalCategory::MODEL, "MODEL"},
    {SignalCategory::ALGORITHM, "ALGORITHM"},
    {SignalCategory::CLASSIFIER, "CLASSIFIER"},
    {SignalCategory::SIGNAL, "SIGNAL"},
    {SignalCategory::SOURCE, "SOURCE"},
};

// NOLINTNEXTLINE(cert-err58-cpp)
const std::map<std::string, SignalCategory> SignalCategoryMapper::toEnum{{"UNKNOWN", SignalCategory::UNKNOWN},
                                                                         {"MODEL", SignalCategory::MODEL},
                                                                         {"ALGORITHM", SignalCategory::ALGORITHM},
                                                                         {"CLASSIFIER", SignalCategory::CLASSIFIER},
                                                                         {"SIGNAL", SignalCategory::SIGNAL},
                                                                         {"SOURCE", SignalCategory::SOURCE}};

SignalCategory SignalCategoryMapper::getEnum(const std::string& category)
{
    try {
        return SignalCategoryMapper::toEnum.at(category);
    } catch (const std::out_of_range&) {
        return SignalCategory::UNKNOWN;
    }
}

std::string SignalCategoryMapper::getString(SignalCategory category)
{
    try {
        return SignalCategoryMapper::toString.at(category);
    } catch (const std::out_of_range&) {
        return "UNKNOWN";
    }
}

void dfx::api::to_json(nlohmann::json& j, const Signal& s)
{
    j = nlohmann::json();
    setJSONFieldIfNotDefault(j, "ID", s.id);
    setJSONFieldIfNotDefault(j, "Name", s.name);
    setJSONFieldIfNotDefault(j, "Version", s.version);
    setJSONFieldIfNotDefault(j, "Description", s.description);
    setJSONFieldIfNotDefault(j, "Unit", s.unit);
    setJSONFieldIfNotDefault(j, "ModelMinAmplitude", s.modelMinAmplitude);
    setJSONFieldIfNotDefault(j, "ModelMaxAmplitude", s.modelMaxAmplitude);
    setJSONFieldIfNotDefault(j, "HumanMinAmplitude", s.humanMinAmplitude);
    setJSONFieldIfNotDefault(j, "HumanMaxAmplitude", s.humanMaxAmplitude);
    setJSONFieldIfNotDefault(j, "InitialDelaySeconds", s.initialDelaySeconds);
    setJSONFieldIfNotDefault(j, "ModelMinSeconds", s.modelMinSeconds);

    if (s.category != SignalCategory::UNKNOWN) {
        j["Category"] = SignalCategoryMapper::getString(s.category);
    }
}

void dfx::api::from_json(const nlohmann::json& j, Signal& s)
{
    getValidField(j, "ID", s.id);
    getValidField(j, "Name", s.name);
    getValidField(j, "Version", s.version);
    getValidField(j, "Description", s.description);
    getValidField(j, "Unit", s.unit);
    getValidField(j, "ModelMinAmplitude", s.modelMinAmplitude);
    getValidField(j, "ModelMaxAmplitude", s.modelMaxAmplitude);
    getValidField(j, "HumanMinAmplitude", s.humanMinAmplitude);
    getValidField(j, "HumanMaxAmplitude", s.humanMaxAmplitude);
    getValidField(j, "InitialDelaySeconds", s.initialDelaySeconds);

    nlohmann::json field;
    if (jsonValidField(j, "Category", field)) {
        const std::string& category = field;
        s.category = SignalCategoryMapper::getEnum(category);
    }
}