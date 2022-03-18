// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_USER_GRPC_H
#define DFX_API_CLOUD_USER_GRPC_H

#include "dfx/api/UserAPI.hpp"

#include "dfx/users/v2/users.grpc.pb.h"

#include <list>
#include <memory>
#include <string>

namespace dfx::api::grpc
{

class CloudGRPC;

class UserGRPC : public UserAPI
{
public:
    UserGRPC(const CloudConfig& config, const std::shared_ptr<CloudGRPC>& cloudGRPC);

    ~UserGRPC() override = default;

    CloudStatus create(const CloudConfig& config,
                       const std::string& email,
                       const std::string& password,
                       const std::string& role,
                       const std::string& firstName,
                       const std::string& lastName,
                       const std::string& phoneNumber,
                       const std::string& gender,
                       const std::string& dateOfBirth,
                       const uint16_t heightCM,
                       const uint16_t weightKG,
                       std::string& userID) override;

    CloudStatus list(const CloudConfig& config,
                     const std::unordered_map<UserFilter, std::string>& filters,
                     uint16_t offset,
                     std::vector<User>& users,
                     int16_t& totalCount) override;

    CloudStatus
    retrieve(const CloudConfig& config, const std::string& userID, const std::string& email, User& user) override;

    CloudStatus update(const CloudConfig& config, const User& user) override;

    CloudStatus remove(const CloudConfig& config, const std::string& userID, const std::string& email) override;

private:
    std::unique_ptr<dfx::users::v2::API::Stub> grpcUserStub;
};

} // namespace dfx::api::grpc

#endif // DFX_API_CLOUD_USER_GRPC_H
