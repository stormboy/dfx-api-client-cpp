// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/websocket/OrganizationWebSocket.hpp"
#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/websocket/CloudWebSocket.hpp"

#include "dfx/proto/organizations.pb.h"

#include "nlohmann/json.hpp"
#include <fmt/format.h>
#include <string>

using namespace dfx::api;
using namespace dfx::api::websocket;
using nlohmann::json;

OrganizationWebSocket::OrganizationWebSocket(const CloudConfig& config, std::shared_ptr<CloudWebSocket> cloudWebSocket)
    : cloudWebSocket(std::move(cloudWebSocket))
{
}

CloudStatus OrganizationWebSocket::create(const CloudConfig& config,
                                          const std::string& name,
                                          const std::string& identifier,
                                          const std::string& public_key,
                                          const OrganizationStatus& status,
                                          const std::string& logo,
                                          std::string& organizationID)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "WebSocket", "organization create"));
}

CloudStatus OrganizationWebSocket::list(const CloudConfig& config,
                                        const std::unordered_map<OrganizationFilter, std::string>& filters,
                                        uint16_t offset,
                                        std::vector<Organization>& organizations,
                                        int16_t& totalCount)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "WebSocket", "organization list"));
}

CloudStatus OrganizationWebSocket::retrieve(const CloudConfig& config,
                                            const std::string& organizationID,
                                            Organization& organization)
{
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, retrieve(config, organizationID, organization));
    dfx::proto::organizations::RetrieveRequest request;
    dfx::proto::organizations::RetrieveResponse response;

    auto status = cloudWebSocket->sendMessage(dfx::api::web::Organizations::Retrieve, request, response);
    if (status.OK()) {
        organization.id = response.id();
        organization.name = response.name();
        organization.identifier = response.identifier();
        organization.status = OrganizationStatusMapper::toEnum.at(response.status());
        organization.contact = response.contact();
        organization.email = response.email();
        organization.createdEpochSeconds = response.created();
        organization.updatedEpochSeconds = response.updated();
        getLogo(config, organizationID, organization.logo);
    }
    return status;
}

CloudStatus OrganizationWebSocket::update(const CloudConfig& config, Organization& organization)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "WebSocket", "organization update"));
}

CloudStatus OrganizationWebSocket::remove(const CloudConfig& config, const std::string& organizationID)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "WebSocket", "organization remove"));
}

CloudStatus OrganizationWebSocket::getLogo(const CloudConfig& config, const std::string& ID, std::string& logo)
{
    dfx::proto::organizations::RetrieveLogoRequest request;
    dfx::proto::organizations::RetrieveLogoResponse response;
    request.mutable_params()->set_id(ID);

    auto status = cloudWebSocket->sendMessage(dfx::api::web::Organizations::RetrieveLogo, request, response);
    if (status.OK()) {
        logo = response.data();
        return status;
    }
    return status;
}
