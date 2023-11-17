// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/websocket/protobuf/UserWebSocketProtobuf.hpp"
#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/websocket/protobuf/CloudWebSocketProtobuf.hpp"

#include "dfx/proto/organizations.pb.h"
#include "dfx/proto/users.pb.h"

using namespace dfx::api;
using namespace dfx::api::websocket::protobuf;

UserWebSocketProtobuf::UserWebSocketProtobuf(const CloudConfig& config,
                                             std::shared_ptr<CloudWebSocketProtobuf> cloudWebSocketProtobuf)
    : cloudWebSocketProtobuf(std::move(cloudWebSocketProtobuf))
{
}

CloudStatus UserWebSocketProtobuf::create(const CloudConfig& config,
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
    dfx::proto::users::CreateRequest request;
    dfx::proto::users::CreateResponse response;

    request.set_firstname(firstName);
    request.set_lastname(lastName);
    request.set_password(password);
    request.set_email(email);
    request.set_gender(gender);
    request.set_dateofbirth(dateOfBirth);
    request.set_roleid(role);
    request.set_phonenumber(phoneNumber);
    request.set_heightcm(heightCM);
    request.set_weightkg(weightKG);

    auto status = cloudWebSocketProtobuf->sendMessage(dfx::api::web::Users::Create, request, response);
    if (status.OK()) {
        userID = response.id();
    }
    return status;
}

CloudStatus UserWebSocketProtobuf::list(const CloudConfig& config,
                                        const std::unordered_map<UserFilter, std::string>& filters,
                                        uint16_t offset,
                                        std::vector<User>& users,
                                        int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, list(config, filters, offset, users, totalCount));
    dfx::proto::organizations::UsersRequest request;
    dfx::proto::organizations::UsersResponse response;

    totalCount = -1;
    auto status = cloudWebSocketProtobuf->sendMessage(dfx::api::web::Organizations::Users, request, response);
    if (status.OK()) {
        for (auto index = 0; index < response.users_size(); index++) {
            const auto& user = response.users(index);
            User u;
            u.id = user.id();
            u.firstName = user.firstname();
            u.lastName = user.lastname();
            u.email = user.email();
            u.gender = user.gender();
            u.dateOfBirth = user.dateofbirth();
            users.push_back(u);
        }
    }

    return status;
}

CloudStatus UserWebSocketProtobuf::retrieve(const CloudConfig& config, User& user)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, retrieve(config, user));
    dfx::proto::users::RetrieveRequest request;
    dfx::proto::users::RetrieveResponse response;

    auto status = cloudWebSocketProtobuf->sendMessage(dfx::api::web::Users::Retrieve, request, response);
    if (status.OK()) {
        user.id = response.id();
        user.firstName = response.firstname();
        user.lastName = response.lastname();
        user.email = response.email();
        user.gender = response.gender();
        user.dateOfBirth = response.dateofbirth();
        user.avatarURL = response.avataruri();
        user.createdEpochSeconds = response.created();
        user.updatedEpochSeconds = response.updated();
        user.role = response.roleid();
        user.phoneNumber = response.phonenumber();
        user.isVerified = response.isverified();
        user.verificationCode = response.verificationcode();
        user.heightCM = response.heightcm();
        user.weightKG = response.weightkg();
        user.loginMethod = response.loginmethod();
        user.ssoID = response.ssoid();
    }
    return status;
}

CloudStatus UserWebSocketProtobuf::update(const CloudConfig& config, const User& user)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, update(config, user));
    dfx::proto::users::UpdateRequest request;
    dfx::proto::users::UpdateResponse response;

    request.set_email(user.email);
    request.set_firstname(user.firstName);
    request.set_lastname(user.lastName);
    request.set_password(user.password);
    request.set_gender(user.gender);
    request.set_dateofbirth(user.dateOfBirth);
    request.set_phonenumber(user.phoneNumber);
    request.set_heightcm(user.heightCM);
    request.set_weightkg(user.weightKG);

    auto status = cloudWebSocketProtobuf->sendMessage(dfx::api::web::Users::Update, request, response);
    return status;
}
