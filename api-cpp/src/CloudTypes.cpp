// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/CloudTypes.hpp"
#include "fmt/format.h"

using namespace dfx::api;
using namespace nlohmann;

bool dfx::api::jsonValidField(const nlohmann::json& j, const std::string& name, nlohmann::json& field)
{
    if (j.contains(name)) {
        field = j.at(name);
        return !field.is_null();
    }
    return false;
}

void dfx::api::getStringifedField(const nlohmann::json& j, const std::string& name, std::string& field)
{
    if (j.contains(name)) {
        nlohmann::json value = j.at(name);
        if (!value.is_null()) {
            field = value.dump();
        }
    }
}
