// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/rest/OrganizationREST.hpp"
#include "dfx/api/rest/CloudREST.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

#include "nlohmann/json.hpp"
#include <fmt/format.h>
#include <string>

using namespace dfx::api;
using namespace dfx::api::rest;

CloudStatus OrganizationREST::create(const CloudConfig& config,
                                     const std::string& name,
                                     const std::string& identifier,
                                     const std::string& public_key,
                                     const OrganizationStatus& status,
                                     const std::string& logo,
                                     std::string& organizationID)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "REST", "organization create"));
}

CloudStatus OrganizationREST::list(const CloudConfig& config,
                                   const std::unordered_map<OrganizationFilter, std::string>& filters,
                                   uint16_t offset,
                                   std::vector<Organization>& organizations,
                                   int16_t& totalCount)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "REST", "organization list"));
}

CloudStatus OrganizationREST::retrieve(const CloudConfig& config,
                                       const std::string& organizationID,
                                       Organization& organization)
{
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, retrieve(config, organizationID, organization));

    nlohmann::json request;
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/organizations/retrieve
    auto result =
        CloudREST::performRESTCall(config, web::Organizations::Retrieve, config.authToken, {}, request, response);

    if (result.OK()) {
        organization = response;
        getLogo(config, organizationID, organization.logo);
    }

    return result;
}

CloudStatus OrganizationREST::update(const CloudConfig& config, Organization& organization)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "REST", "organization update"));
}

CloudStatus OrganizationREST::remove(const CloudConfig& config, const std::string& organizationID)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "REST", "organization remove"));
}

CloudStatus OrganizationREST::getLogo(const CloudConfig& config, const std::string& ID, std::string& logo)
{
    nlohmann::json request;
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/organizations/logo
    auto result =
        CloudREST::performRESTCall(config, web::Organizations::RetrieveLogo, config.authToken, {ID}, request, response);

    if (result.OK()) {
        logo = response;
    }

    return result;
}

CloudStatus OrganizationREST::listUsers(const CloudConfig& config,
                      const std::unordered_map<dfx::api::UserAPI::UserFilter, std::string>& filters,
                      uint16_t offset,
                      std::vector<User>& users,
                      int16_t& totalCount) {
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, listUsers(config, filters, offset, users, totalCount));

    nlohmann::json request;
    nlohmann::json response;

    auto result = CloudREST::performRESTCall(
        config, web::Organizations::Users, config.authToken, {}, request, response);

    if (result.OK()) {
        users = response;
    }

    return result;
}

CloudStatus OrganizationREST::createUser(const CloudConfig& config, User& user) {
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, createUser(config, user));

    nlohmann::json request = user;
    nlohmann::json response;

    auto result = CloudREST::performRESTCall(
        config, web::Organizations::CreateUser, config.authToken, {}, request, response);

    if (result.OK()) {
        user = response;
    }

    return result;
}

CloudStatus
OrganizationREST::retrieveUser(const CloudConfig& config, const std::string& userID, const std::string& email, User& user)
{
    // Ignore email check for empty, REST call is going to ignore it anyway
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, retrieve(config, userID, "email", user));

    nlohmann::json request;
    nlohmann::json response;

    auto result = CloudREST::performRESTCall(
        config, web::Organizations::RetrieveUser, config.authToken, {userID}, request, response);

    if (result.OK()) {
        user = response;
    }

    return result;
}

CloudStatus
OrganizationREST::updateUser(const CloudConfig& config, const std::string& userID, const std::string& email, const User& user)
{
    // Ignore email check for empty, REST call is going to ignore it anyway
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, update(config, userID, "email", user));

    nlohmann::json request;
    nlohmann::json response;

    request = user;
    auto result = CloudREST::performRESTCall(
        config, web::Organizations::UpdateUser, config.authToken, {user.id}, request, response);

    return result;
}

CloudStatus OrganizationREST::removeUser(const CloudConfig& config, const std::string& userID, const std::string& email)
{
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, remove(config, userID, email));

    nlohmann::json request;
    nlohmann::json response;

    auto result = CloudREST::performRESTCall(
        config, web::Organizations::RemoveUser, config.authToken, {userID}, request, response);

    return result;
}
