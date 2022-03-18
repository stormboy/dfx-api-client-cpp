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
using nlohmann::json;

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

    json response, request;
    auto status =
        CloudREST::performRESTCall(config, web::Organizations::Retrieve, config.authToken, {}, request, response);

    if (status.OK()) {
        organization = response;
        getLogo(config, organizationID, organization.logo);
    }
    return status;
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
    json response, request;
    auto status =
        CloudREST::performRESTCall(config, web::Organizations::RetrieveLogo, config.authToken, {ID}, request, response);

    if (status.OK()) {
        logo = response;
    }
    return status;
}
