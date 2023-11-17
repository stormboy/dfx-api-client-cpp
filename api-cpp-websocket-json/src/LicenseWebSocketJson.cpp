// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/websocket/json/LicenseWebSocketJson.hpp"
#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/websocket/json/CloudWebSocketJson.hpp"

#include "nlohmann/json.hpp"
#include <fmt/format.h>
#include <string>

using namespace dfx::api;
using namespace dfx::api::websocket::json;

LicenseWebSocketJson::LicenseWebSocketJson(const CloudConfig& config,
                                           std::shared_ptr<CloudWebSocketJson> cloudWebSocketJson)
    : cloudWebSocketJson(std::move(cloudWebSocketJson))
{
}

CloudStatus LicenseWebSocketJson::list(const CloudConfig& config,
                                       const std::unordered_map<LicenseFilter, std::string>& filters,
                                       uint16_t offset,
                                       std::vector<License>& licenses,
                                       int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(LicenseValidator, list(config, filters, offset, licenses, totalCount));

    totalCount = -1; // Return unknown -1, zero would be a literal zero

    if (filters.size() > 0) {
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "Unexpected list filter key");
    }

    nlohmann::json request;
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/licenses/get-organization-licenses
    auto result =
        cloudWebSocketJson->sendMessageJson(config, web::Licenses::ListOrgLicenses, {}, {}, request, response);
    if (result.OK()) {
        std::vector<License> licensesTemp = response;
        licenses.insert(licenses.end(), licensesTemp.begin(), licensesTemp.end());

        if (licensesTemp.size() > 0) {
            // First element assuming there is one will have a TotalCount field which makes for
            // a non-uniform JSON schema so custom decode here
            if (response[0].contains("TotalCount")) {
                totalCount = response[0]["TotalCount"];
            }
        }
    }

    return result;
}