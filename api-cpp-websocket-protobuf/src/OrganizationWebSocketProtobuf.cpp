// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/websocket/protobuf/OrganizationWebSocketProtobuf.hpp"
#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/websocket/protobuf/CloudWebSocketProtobuf.hpp"

#include "dfx/proto/organizations.pb.h"

#include "nlohmann/json.hpp"
#include <fmt/format.h>
#include <string>

using namespace dfx::api;
using namespace dfx::api::websocket::protobuf;
using nlohmann::json;

OrganizationWebSocketProtobuf::OrganizationWebSocketProtobuf(
    const CloudConfig& config, std::shared_ptr<CloudWebSocketProtobuf> cloudWebSocketProtobuf)
    : cloudWebSocketProtobuf(std::move(cloudWebSocketProtobuf))
{
}

CloudStatus OrganizationWebSocketProtobuf::create(const CloudConfig& config,
                                                  const std::string& name,
                                                  const std::string& identifier,
                                                  const std::string& public_key,
                                                  const OrganizationStatus& status,
                                                  const std::string& logo,
                                                  std::string& organizationID)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "WebSocketProtobuf", "organization create"));
}

CloudStatus OrganizationWebSocketProtobuf::list(const CloudConfig& config,
                                                const std::unordered_map<OrganizationFilter, std::string>& filters,
                                                uint16_t offset,
                                                std::vector<Organization>& organizations,
                                                int16_t& totalCount)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "WebSocketProtobuf", "organization list"));
}

CloudStatus OrganizationWebSocketProtobuf::retrieve(const CloudConfig& config,
                                                    const std::string& organizationID,
                                                    Organization& organization)
{
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, retrieve(config, organizationID, organization));
    dfx::proto::organizations::RetrieveRequest request;
    dfx::proto::organizations::RetrieveResponse response;

    auto status = cloudWebSocketProtobuf->sendMessage(dfx::api::web::Organizations::Retrieve, request, response);
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

CloudStatus OrganizationWebSocketProtobuf::update(const CloudConfig& config, Organization& organization)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "WebSocketProtobuf", "organization update"));
}

CloudStatus OrganizationWebSocketProtobuf::remove(const CloudConfig& config, const std::string& organizationID)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "WebSocketProtobuf", "organization remove"));
}

CloudStatus OrganizationWebSocketProtobuf::getLogo(const CloudConfig& config, const std::string& ID, std::string& logo)
{
    dfx::proto::organizations::RetrieveLogoRequest request;
    dfx::proto::organizations::RetrieveLogoResponse response;
    request.mutable_params()->set_id(ID);

    auto status = cloudWebSocketProtobuf->sendMessage(dfx::api::web::Organizations::RetrieveLogo, request, response);
    if (status.OK()) {
        logo = response.data();
        return status;
    }
    return status;
}

CloudStatus OrganizationWebSocketProtobuf::listUsers(const CloudConfig& config,
                                                 const std::unordered_map<dfx::api::UserAPI::UserFilter, std::string>& filters,
                                                 uint16_t offset,
                                                 std::vector<User>& users,
                                                 int16_t& totalCount) {
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, listUsers(config, filters, offset, users, totalCount));

    dfx::proto::organizations::UsersRequest request;
    dfx::proto::organizations::UsersResponse response;

    auto result = cloudWebSocketProtobuf->sendMessage( web::Organizations::Users, request, response);

    if (result.OK()) {
        for (const auto &userProto: response.users() ) {
            User user;
            user.id = userProto.id();
            user.firstName = userProto.firstname();
            user.lastName = userProto.lastname();
            user.email = userProto.email();
            user.gender = userProto.gender();
            user.dateOfBirth = userProto.dateofbirth();
            users.push_back(user);
        }
    }
    return result;
}

CloudStatus OrganizationWebSocketProtobuf::createUser(const CloudConfig& config, User& user) {
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE);
}

CloudStatus OrganizationWebSocketProtobuf::retrieveUser(const CloudConfig& config,
                                            const std::string& userID,
                                            const std::string& email,
                                            User& user)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE);
}

CloudStatus OrganizationWebSocketProtobuf::updateUser(const CloudConfig& config,
                                          const std::string& userID,
                                          const std::string& email,
                                          const User& user)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE);
}

CloudStatus OrganizationWebSocketProtobuf::removeUser(const CloudConfig& config,
                                          const std::string& email,
                                          const std::string& userID)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE);
}