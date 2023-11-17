// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/websocket/json/OrganizationWebSocketJson.hpp"
#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/websocket/json/CloudWebSocketJson.hpp"

#include "nlohmann/json.hpp"
#include <fmt/format.h>
#include <string>

using namespace dfx::api;
using namespace dfx::api::websocket::json;

OrganizationWebSocketJson::OrganizationWebSocketJson(const CloudConfig& config,
                                                     std::shared_ptr<CloudWebSocketJson> cloudWebSocketJson)
    : cloudWebSocketJson(std::move(cloudWebSocketJson))
{
}

CloudStatus OrganizationWebSocketJson::create(const CloudConfig& config,
                                              const std::string& name,
                                              const std::string& identifier,
                                              const std::string& public_key,
                                              const OrganizationStatus& status,
                                              const std::string& logo,
                                              std::string& organizationID)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "WebSocketJson", "organization create"));
}

CloudStatus OrganizationWebSocketJson::list(const CloudConfig& config,
                                            const std::unordered_map<OrganizationFilter, std::string>& filters,
                                            uint16_t offset,
                                            std::vector<Organization>& organizations,
                                            int16_t& totalCount)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "WebSocketJson", "organization list"));
}

CloudStatus OrganizationWebSocketJson::retrieve(const CloudConfig& config,
                                                const std::string& organizationID,
                                                Organization& organization)
{
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, retrieve(config, organizationID, organization));

    nlohmann::json request;
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/organizations/retrieve
    auto result = cloudWebSocketJson->sendMessageJson(config, web::Organizations::Retrieve, {}, {}, request, response);

    if (result.OK()) {
        organization = response;
        getLogo(config, organizationID, organization.logo);
    }

    return result;
}

CloudStatus OrganizationWebSocketJson::update(const CloudConfig& config, Organization& organization)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "WebSocketJson", "organization update"));
}

CloudStatus OrganizationWebSocketJson::remove(const CloudConfig& config, const std::string& organizationID)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "WebSocketJson", "organization remove"));
}

CloudStatus OrganizationWebSocketJson::getLogo(const CloudConfig& config, const std::string& ID, std::string& logo)
{
    nlohmann::json request;
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/organizations/logo
    auto result =
        cloudWebSocketJson->sendMessageJson(config, web::Organizations::RetrieveLogo, {}, {}, request, response);

    if (result.OK()) {
        logo = response;
    }

    return result;
}

CloudStatus OrganizationWebSocketJson::listUsers(const CloudConfig& config,
                                        const std::unordered_map<dfx::api::UserAPI::UserFilter, std::string>& filters,
                                        uint16_t offset,
                                        std::vector<User>& users,
                                        int16_t& totalCount) {
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, listUsers(config, filters, offset, users, totalCount));

    nlohmann::json request;
    nlohmann::json response;

    auto result = cloudWebSocketJson->sendMessageJson(
        config, web::Organizations::Users, {}, {}, request, response);

    if (result.OK()) {
        users = response;
    }

    return result;
}

CloudStatus OrganizationWebSocketJson::createUser(const CloudConfig& config, User& user) {
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE);
}

CloudStatus
OrganizationWebSocketJson::retrieveUser(const CloudConfig& config, const std::string& userID, const std::string& email, User& user)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "WebSocketJson", "user retrieve"));
}

CloudStatus OrganizationWebSocketJson::updateUser(const CloudConfig& config,
                                      const std::string& userID,
                                      const std::string& email,
                                      const User& user)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "WebSocketJson", "user update"));
}

CloudStatus OrganizationWebSocketJson::removeUser(const CloudConfig& config, const std::string& email, const std::string& userID)
{
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, remove(config, email, userID));

    nlohmann::json request;
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/users/remove
    auto result = cloudWebSocketJson->sendMessageJson(config, web::Users::Remove,
                                                      {}, {}, request, response);
    return result;
}