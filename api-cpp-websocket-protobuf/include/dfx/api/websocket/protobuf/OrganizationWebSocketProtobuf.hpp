// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_ORGANIZATIOH_WEBSOCKET_PROTOBUF_H
#define DFX_API_CLOUD_ORGANIZATIOH_WEBSOCKET_PROTOBUF_H

#include "dfx/api/OrganizationAPI.hpp"

namespace dfx::api::websocket::protobuf
{

class CloudWebSocketProtobuf;

class OrganizationWebSocketProtobuf : public OrganizationAPI
{
public:
    OrganizationWebSocketProtobuf(const CloudConfig& config,
                                  std::shared_ptr<CloudWebSocketProtobuf> cloudWebSocketProtobuf);

    ~OrganizationWebSocketProtobuf() override = default;

    CloudStatus create(const CloudConfig& config,
                       const std::string& name,
                       const std::string& identifier,
                       const std::string& public_key,
                       const OrganizationStatus& status,
                       const std::string& logo,
                       std::string& organizationID) override;

    CloudStatus list(const CloudConfig& config,
                     const std::unordered_map<OrganizationFilter, std::string>& filters,
                     uint16_t offset,
                     std::vector<Organization>& organizations,
                     int16_t& totalCount) override;

    CloudStatus retrieve(const CloudConfig& config,
                         const std::string& organizationID,
                         Organization& organization) override;

    CloudStatus update(const CloudConfig& config, Organization& organization) override;

    CloudStatus remove(const CloudConfig& config, const std::string& organizationID) override;

    CloudStatus listUsers(const CloudConfig& config,
                          const std::unordered_map<dfx::api::UserAPI::UserFilter, std::string>& filters,
                          uint16_t offset,
                          std::vector<User>& users,
                          int16_t& totalCount) override;

    CloudStatus createUser(const CloudConfig& config, User& user) override;

    CloudStatus
    retrieveUser(const CloudConfig& config, const std::string& userID, const std::string& email, User& user) override;

    CloudStatus
    updateUser(const CloudConfig& config, const std::string& userID, const std::string& email, const User& user) override;

    CloudStatus removeUser(const CloudConfig& config, const std::string& email, const std::string& userID) override;

private:
    CloudStatus getLogo(const CloudConfig& config, const std::string& ID, std::string& logo);

private:
    std::shared_ptr<CloudWebSocketProtobuf> cloudWebSocketProtobuf;
};

} // namespace dfx::api::websocket::protobuf

#endif // DFX_API_CLOUD_ORGANIZATIOH_WEBSOCKET_PROTOBUF_H
