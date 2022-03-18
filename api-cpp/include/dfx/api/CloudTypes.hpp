// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#ifndef DFX_API_CLOUD_TYPES_H
#define DFX_API_CLOUD_TYPES_H

#include <cstdint>

#include <ostream>

#include "CloudStatus.hpp"

#include <nlohmann/json.hpp>

namespace dfx::api
{

bool jsonValidField(const nlohmann::json& j, const std::string& name, nlohmann::json& field);

template <typename T>
void getValidField(const nlohmann::json& j, const std::string& name, T& field)
{
    if (j.contains(name)) {
        nlohmann::json value = j.at(name);
        if (!value.is_null()) {
            value.get_to(field);
        }
    }
}

template <typename T>
void setJSONFieldIfNotDefault(nlohmann::json& j, const std::string& name, T& field)
{
    static T emptyValue{};
    if (field != emptyValue) {
        j[name] = field;
    }
}

void getStringifedField(const nlohmann::json& j, const std::string& name, std::string& field);

} // namespace dfx::api

#endif // DFX_API_CLOUD_TYPES_H
