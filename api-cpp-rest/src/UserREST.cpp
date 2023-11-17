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

    nlohmann::json request;
    nlohmann::json response;

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

    auto result =
        CloudREST::performRESTCall(config, web::Organizations::CreateUser, config.authToken, {}, request, response);

    if (result.OK()) {
        userID = response["ID"];
    }

    return result;
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

    nlohmann::json request;
    nlohmann::json response;

    auto result = CloudREST::performRESTCall(config, web::Users::Retrieve, config.authToken, {}, request, response);
    if (result.OK()) {
        user = response;
    }

    return result;
}

CloudStatus UserREST::update(const CloudConfig& config, const User& user)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, update(config, user));

    nlohmann::json request;
    nlohmann::json response;

    request = user;
    auto result = CloudREST::performRESTCall(config, web::Users::Update, config.authToken, {}, request, response);

    return result;
}

CloudStatus UserREST::reqLoginCode(const CloudConfig& config, const std::string& sOrgKey, const std::string& sPhoNum)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, reqLoginCode(config, sOrgKey, sPhoNum));

    nlohmann::json request;
    nlohmann::json response;

    auto result = CloudREST::performRESTCall(
        config, web::Users::RequestLoginCode, config.authToken, {sOrgKey, sPhoNum}, request, response);

    return result;
}

CloudStatus UserREST::loginWithPhoneCode(CloudConfig& config,
                                         const std::string& sOrgKey,
                                         const std::string& sPhoNum,
                                         const std::string& sPhoCode)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, loginWithPhoneCode(config, sOrgKey, sPhoNum, sPhoCode));

    nlohmann::json request;
    nlohmann::json response;

    request["LoginCode"] = sPhoCode;
    request["PhoneNumber"] = sPhoNum;
    request["OrgKey"] = sOrgKey;

    auto result =
        CloudREST::performRESTCall(config, web::Users::LoginWithCode, config.authToken, {}, request, response);

    if (result.OK()) {
        config.authToken = response["Token"];
    }

    return result;
}

CloudStatus UserREST::retrieveUserRole(const CloudConfig& config, UserRole& userRole)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, retrieveUserRole(config, userRole));

    nlohmann::json request;
    nlohmann::json response;

    auto result = CloudREST::performRESTCall(config, web::Users::GetRole, config.authToken, {}, request, response);

    if (result.OK()) {
        userRole.id = response["ID"];
        userRole.name = response["Name"];
        userRole.description = response["Description"];
        userRole.organization = response["Organization"];
    }

    return result;
}

CloudStatus UserREST::sendPasswordReset(const CloudConfig& config,
                                        const std::string& sIdentifier,
                                        std::string& sResetToken)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, sendPasswordReset(config, sIdentifier, sResetToken));

    nlohmann::json request;
    nlohmann::json response;

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

    nlohmann::json request;
    nlohmann::json response;

    request["ResetToken"] = sResetToken;
    request["Password"] = sPassword;

    auto result =
        CloudREST::performRESTCall(config, web::Users::ResetPassword, config.authToken, {}, request, response);

    return result;
}

CloudStatus UserREST::sendVerificationCode(const CloudConfig& config,
                                           const std::string& sUserID,
                                           const std::string& sOrgKey)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, sendVerificationCode(config, sUserID, sOrgKey));

    nlohmann::json request;
    nlohmann::json response;

    auto result = CloudREST::performRESTCall(
        config, web::Users::VerificationCode, config.authToken, {sUserID, sOrgKey}, request, response);

    return result;
}

CloudStatus UserREST::verifyAccount(const CloudConfig& config, const std::string& sUserID, const std::string& sVerCode)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, verifyAccount(config, sUserID, sVerCode));

    nlohmann::json request;
    nlohmann::json response;

    request["VerificationCode"] = sVerCode;
    request["ID"] = sUserID;

    auto result = CloudREST::performRESTCall(config, web::Users::Verify, config.authToken, {}, request, response);

    return result;
}
