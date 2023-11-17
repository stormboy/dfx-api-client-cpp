// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/OrganizationAPI.hpp"

#include "dfx/api/grpc/CloudGRPC.hpp"
#include "dfx/api/grpc/OrganizationGRPC.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

#include "dfx/api/types/FilterHelpers.hpp"

#include "CloudGRPCMacros.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>

using dfx::api::CloudAPI;
using dfx::api::CloudConfig;
using dfx::api::CloudStatus;
using dfx::api::Measurement;
using dfx::api::MeasurementAPI;

using namespace dfx::api::grpc;
using namespace ::grpc;

// Helpers for Birth Date which in gRPC is a EPOCH time, but WebSocket/REST is a %F string.
// Internally, Cloud API treats birth date as a string.
std::string epocToString(uint64_t when)
{
    time_t epochtime = when;
    struct tm date;
    date = *std::localtime(&epochtime);
    char buffer[256];
    strftime(buffer, sizeof(buffer), "%F", &date); // %F = Short YYYY-MM-DD, equivalent to %Y-%m-%d  2001-08-23
    return std::string(buffer);
}

uint64_t strToEpoch(const std::string& str)
{
    std::tm tmTime = {};
    std::istringstream ss(str);
    ss >> std::get_time(&tmTime, "%Y-%m-%d");
    return mktime(&tmTime);
}

OrganizationGRPC::OrganizationGRPC(const CloudConfig& config, const std::shared_ptr<CloudGRPC>& cloudGRPC)
{
    grpcUserStub = dfx::users::v2::API::NewStub(cloudGRPC->getChannel(config));
}

CloudStatus OrganizationGRPC::listUsers(const CloudConfig& config,
                      const std::unordered_map<dfx::api::UserAPI::UserFilter, std::string>& filters,
                      uint16_t offset,
                      std::vector<User>& users,
                      int16_t& totalCount) {
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, listUsers(config, filters, offset, users, totalCount));

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
        auto status = retrieveUser(config, userData.id(), userData.email(), user);
        if (!status.OK()) {
            return status;
        }

        userList.push_back(user);
    }

    users.insert(users.end(), userList.begin(), userList.end());
    totalCount = response.total();

    return CloudStatus(CLOUD_OK);
}

CloudStatus OrganizationGRPC::createUser(const CloudConfig& config, User& user) {
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, createUser(config, user));

    dfx::users::v2::CreateResponse response;
    dfx::users::v2::CreateRequest request;
    request.set_email(user.email);
    request.set_role(user.role);
    request.set_password(user.password);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcUserStub->Create(&context, request, &response));

    return CloudStatus(CLOUD_OK);
}

CloudStatus
OrganizationGRPC::retrieveUser(const CloudConfig& config, const std::string& userID, const std::string& email, User& user)
{
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, retrieveUser(config, userID, email, user));

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
        user.dateOfBirth = epocToString(userData.date_of_birth().seconds());
    }

    return CloudStatus(CLOUD_OK);
}

CloudStatus OrganizationGRPC::updateUser(const CloudConfig& config, const std::string& userID, const std::string& email, const User& user) {
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, updateUser(config, userID, email, user));

    dfx::users::v2::UpdateResponse response;
    dfx::users::v2::UpdateRequest request;
    request.set_id(userID);
    request.set_first_name(user.firstName);
    request.set_last_name(user.lastName);
    request.set_email(email);
    request.set_role(user.role);
    request.set_password(user.password);
    request.set_height_cm(user.heightCM);
    request.set_weight_kg(user.weightKG);
    request.set_gender(user.gender);
    request.set_status(static_cast<dfx::users::v2::Status>(user.status));
    request.set_avatar_uri(user.avatarURL);
    request.mutable_date_of_birth()->set_seconds(static_cast<::google::protobuf::int64>(strToEpoch(user.dateOfBirth)));

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcUserStub->Update(&context, request, &response));

    return CloudStatus(CLOUD_OK);
}

CloudStatus OrganizationGRPC::removeUser(const CloudConfig& config, const std::string& userID, const std::string& email) {
    DFX_CLOUD_VALIDATOR_MACRO(OrganizationValidator, removeUser(config, userID, email));

    dfx::users::v2::RemoveResponse response;
    dfx::users::v2::RemoveRequest request;
    request.set_id(userID);
    request.set_email(email);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcUserStub->Remove(&context, request, &response));

    return CloudStatus(CLOUD_OK);
}
