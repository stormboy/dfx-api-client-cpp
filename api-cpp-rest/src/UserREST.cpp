// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/rest/UserREST.hpp"
#include "dfx/api/rest/CloudREST.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

#include "fmt/format.h"
#include "nlohmann/json.hpp"
#include <string>

using namespace dfx::api;
using namespace dfx::api::rest;
using nlohmann::json;

UserREST::UserREST(const CloudConfig& config, const std::shared_ptr<CloudREST>& cloudREST) {}

CloudStatus UserREST::create(const CloudConfig& config,
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

    json response, request;

    request["FirstName"] = firstName;
    request["LastName"] = lastName;
    request["Email"] = email;
    request["Password"] = password;
    request["Role"] = role;
    request["PhoneNumber"] = phoneNumber;
    request["Gender"] = gender;
    request["DateOfBirth"] = dateOfBirth;
    request["HeightCm"] = heightCM;
    request["WeightKg"] = weightKG;

    auto status =
        CloudREST::performRESTCall(config, web::Organizations::CreateUser, config.authToken, {}, request, response);

    if (status.OK()) {
        userID = response["ID"];
    }

    return status;
}

CloudStatus UserREST::list(const CloudConfig& config,
                           const std::unordered_map<UserFilter, std::string>& filters,
                           uint16_t offset,
                           std::vector<User>& users,
                           int16_t& totalCount)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE, fmt::format("{} does not support {} end-point", "REST", "user list"));
}

CloudStatus UserREST::retrieve(const CloudConfig& config, User& user)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, retrieve(config, user));
    json response, request;
    auto status = CloudREST::performRESTCall(config, web::Users::Retrieve, config.authToken, {}, request, response);
    if (status.OK()) {
        user = response;
    }

    return status;
}

CloudStatus UserREST::update(const CloudConfig& config, const User& user)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, update(config, user));

    json response, request;
    request = user;
    auto status = CloudREST::performRESTCall(config, web::Users::Update, config.authToken, {}, request, response);

    return status;
}

CloudStatus
UserREST::retrieve(const CloudConfig& config, const std::string& userID, const std::string& email, User& user)
{
    // Ignore email check for empty, REST call is going to ignore it anyway
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, retrieve(config, userID, "email", user));
    json response, request;
    auto status = CloudREST::performRESTCall(
        config, web::Organizations::RetrieveUser, config.authToken, {userID}, request, response);
    if (status.OK()) {
        user = response;
    }

    return status;
}

CloudStatus
UserREST::update(const CloudConfig& config, const std::string& userID, const std::string& email, const User& user)
{
    // Ignore email check for empty, REST call is going to ignore it anyway
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, update(config, userID, "email", user));

    json response, request;
    request = user;
    auto status = CloudREST::performRESTCall(
        config, web::Organizations::UpdateUser, config.authToken, {user.id}, request, response);

    return status;
}

CloudStatus UserREST::remove(const CloudConfig& config, const std::string& userID, const std::string& email)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, remove(config, userID, email));

    json response, request;

    auto status = CloudREST::performRESTCall(
        config, web::Organizations::RemoveUser, config.authToken, {userID}, request, response);

    return status;
}

CloudStatus UserREST::reqLoginCode(const CloudConfig& config, const std::string& sOrgKey, const std::string& sPhoNum)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, reqLoginCode(config, sOrgKey, sPhoNum));

    json response, request;

    auto status = CloudREST::performRESTCall(
        config, web::Users::RequestLoginCode, config.authToken, {sOrgKey, sPhoNum}, request, response);

    return status;
}

CloudStatus UserREST::loginWithPhoneCode(CloudConfig& config,
                                         const std::string& sOrgKey,
                                         const std::string& sPhoNum,
                                         const std::string& sPhoCode)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, loginWithPhoneCode(config, sOrgKey, sPhoNum, sPhoCode));

    json response, request;

    request["LoginCode"] = sPhoCode;
    request["PhoneNumber"] = sPhoNum;
    request["OrgKey"] = sOrgKey;

    auto status =
        CloudREST::performRESTCall(config, web::Users::LoginWithCode, config.authToken, {}, request, response);

    if (status.OK()) {
        config.authToken = response["Token"];
    }

    return status;
}

CloudStatus UserREST::retrieveUserRole(const CloudConfig& config, UserRole& userRole)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, retrieveUserRole(config, userRole));

    json response, request;

    auto status = CloudREST::performRESTCall(config, web::Users::GetRole, config.authToken, {}, request, response);

    if (status.OK()) {
        userRole.id = response["ID"];
        userRole.name = response["Name"];
        userRole.description = response["Description"];
        userRole.organization = response["Organization"];
    }

    return status;
}

CloudStatus UserREST::sendPasswordReset(const CloudConfig& config,
                                        const std::string& sIdentifier,
                                        std::string& sResetToken)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, sendPasswordReset(config, sIdentifier, sResetToken));

    json response, request;

    request["Email"] = config.authEmail;
    request["Identifier"] = sIdentifier;

    auto status = CloudREST::performRESTCall(config, web::Users::SendReset, config.authToken, {}, request, response);

    if (status.OK()) {
        sResetToken = response["ResetToken"];
    }

    return status;
}

CloudStatus UserREST::resetPassword(const CloudConfig& config,
                                    const std::string& sPassword,
                                    const std::string& sResetToken)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, resetPassword(config, sPassword, sResetToken));

    json response, request;

    request["ResetToken"] = sResetToken;
    request["Password"] = sPassword;

    auto status =
        CloudREST::performRESTCall(config, web::Users::ResetPassword, config.authToken, {}, request, response);

    return status;
}

CloudStatus UserREST::sendVerificationCode(const CloudConfig& config,
                                           const std::string& sUserID,
                                           const std::string& sOrgKey)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, sendVerificationCode(config, sUserID, sOrgKey));

    json response, request;

    auto status = CloudREST::performRESTCall(
        config, web::Users::VerificationCode, config.authToken, {sUserID, sOrgKey}, request, response);

    return status;
}

CloudStatus UserREST::verifyAccount(const CloudConfig& config, const std::string& sUserID, const std::string& sVerCode)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, verifyAccount(config, sUserID, sVerCode));

    json response, request;

    request["VerificationCode"] = sVerCode;
    request["ID"] = sUserID;

    auto status = CloudREST::performRESTCall(config, web::Users::Verify, config.authToken, {}, request, response);

    return status;
}
