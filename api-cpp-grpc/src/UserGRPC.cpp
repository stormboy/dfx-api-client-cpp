// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/UserAPI.hpp"

#include "dfx/api/grpc/CloudGRPC.hpp"
#include "dfx/api/grpc/UserGRPC.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

#include "dfx/users/v2/users.pb.h"

#include "CloudGRPCMacros.hpp"
#include <fmt/format.h>

using dfx::api::CloudAPI;
using dfx::api::CloudConfig;
using dfx::api::CloudStatus;
using dfx::api::User;
using dfx::api::UserAPI;

using namespace dfx::api::grpc;
using namespace ::grpc;

UserGRPC::UserGRPC(const CloudConfig& config, const std::shared_ptr<CloudGRPC>& cloudGRPC)
{
    grpcUserStub = dfx::users::v2::API::NewStub(cloudGRPC->getChannel(config));
}

CloudStatus UserGRPC::create(const CloudConfig& config,
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
                             std::string& userID)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator,
                              create(config,
                                     email,
                                     password,
                                     role,
                                     firstName,
                                     lastName,
                                     phoneNumber,
                                     gender,
                                     dateOfBirth,
                                     heightCM,
                                     weightKG,
                                     userID));

    dfx::users::v2::CreateResponse response;
    dfx::users::v2::CreateRequest request;
    request.set_email(email);
    request.set_role(role);
    request.set_password(password);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcUserStub->Create(&context, request, &response));

    userID = response.id();

    return CloudStatus(CLOUD_OK);
}

CloudStatus UserGRPC::list(const CloudConfig& config,
                           const std::unordered_map<UserFilter, std::string>& filters,
                           uint16_t offset,
                           std::vector<User>& users,
                           int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, list(config, filters, offset, users, totalCount));

    if (filters.size() > 0) {
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "Unexpected list filter key");
    }

    dfx::users::v2::ListResponse response;
    dfx::users::v2::ListRequest request;
    request.set_limit(config.listLimit);
    request.set_offset(offset);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcUserStub->List(&context, request, &response));
    totalCount = response.total();

    const auto numberUsers = response.users_size();

    std::vector<User> userList;
    userList.reserve(numberUsers);
    for (size_t index = 0; index < numberUsers; index++) {
        const auto& userData = response.users(static_cast<int>(index));

        User user;
        auto status = retrieve(config, userData.id(), userData.email(), user);
        if (!status.OK()) {
            return status;
        }

        userList.push_back(user);
    }

    users.insert(users.end(), userList.begin(), userList.end());
    totalCount = response.total();

    return CloudStatus(CLOUD_OK);
}

CloudStatus
UserGRPC::retrieve(const CloudConfig& config, const std::string& userID, const std::string& email, User& user)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, retrieve(config, userID, email, user));

    dfx::users::v2::RetrieveResponse response;
    dfx::users::v2::RetrieveRequest request;
    request.set_email(email);
    request.set_id(userID);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcUserStub->Retrieve(&context, request, &response));

    if (response.has_user()) {
        const auto& userData = response.user();

        user.id = userData.id();
        user.firstName = userData.first_name();
        user.lastName = userData.last_name();
        user.email = userData.email();
        user.status = static_cast<UserStatus>(userData.status());
        user.gender = userData.gender();
        user.heightCM = userData.height_cm();
        user.weightKG = userData.weight_kg();
        user.avatarURL = userData.avatar_uri();
        user.createdEpochSeconds = userData.created().seconds();
        user.updatedEpochSeconds = userData.updated().seconds();
    }

    return CloudStatus(CLOUD_OK);
}

CloudStatus UserGRPC::update(const CloudConfig& config, const User& user)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, update(config, user));

    dfx::users::v2::UpdateResponse response;
    dfx::users::v2::UpdateRequest request;
    request.set_id(user.id);
    request.set_first_name(user.firstName);
    request.set_last_name(user.lastName);
    request.set_email(user.email);
    request.set_role(user.role);
    request.set_password(user.password);
    request.set_height_cm(user.heightCM);
    request.set_weight_kg(user.weightKG);
    request.set_gender(user.gender);
    request.set_status(static_cast<dfx::users::v2::Status>(user.status));
    request.set_avatar_uri(user.avatarURL);
    request.mutable_date_of_birth()->set_seconds(static_cast<::google::protobuf::int64>(user.dateOfBirthEpochSeconds));

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcUserStub->Update(&context, request, &response));

    return CloudStatus(CLOUD_OK);
}

CloudStatus UserGRPC::remove(const CloudConfig& config, const std::string& userID, const std::string& email)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, remove(config, userID, email));

    dfx::users::v2::RemoveResponse response;
    dfx::users::v2::RemoveRequest request;
    request.set_email(email);
    request.set_id(userID);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcUserStub->Remove(&context, request, &response));

    return CloudStatus(CLOUD_OK);
}
