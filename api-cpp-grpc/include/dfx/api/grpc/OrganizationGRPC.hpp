// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#ifndef DFXCLOUD_ORGANIZATIONGRPC_HPP
#define DFXCLOUD_ORGANIZATIONGRPC_HPP


#include "dfx/api/OrganizationAPI.hpp"

#include "dfx/users/v2/users.grpc.pb.h"

namespace dfx::api::grpc
{

class CloudGRPC;

class OrganizationGRPC : public OrganizationAPI
{
public:
    OrganizationGRPC(const CloudConfig& config, const std::shared_ptr<CloudGRPC>& cloudGRPC);

    ~OrganizationGRPC() override = default;

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

    CloudStatus removeUser(const CloudConfig& config, const std::string& userID, const std::string& email) override;

private:
    std::unique_ptr<dfx::users::v2::API::Stub> grpcUserStub;
};

} // namespace dfx::api::grpc

#endif // DFXCLOUD_ORGANIZATIONGRPC_HPP
