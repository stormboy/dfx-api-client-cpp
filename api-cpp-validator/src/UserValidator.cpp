// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/validator/UserValidator.hpp"

#include "CloudValidatorMacros.hpp"

using namespace dfx::api;
using namespace dfx::api::validator;

const UserValidator& UserValidator::instance()
{
    static const UserValidator instance;
    return instance;
}

CloudStatus UserValidator::create(const CloudConfig& config,
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
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(email);
    MACRO_RETURN_ERROR_IF_EMPTY(firstName);
    MACRO_RETURN_ERROR_IF_EMPTY(password);
    MACRO_RETURN_ERROR_IF_EMPTY(role);
    return CloudStatus(CLOUD_OK);
}

CloudStatus UserValidator::list(const CloudConfig& config,
                                const std::unordered_map<UserFilter, std::string>& filters,
                                uint16_t offset,
                                std::vector<User>& users,
                                int16_t& totalCount)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    return CloudStatus(CLOUD_OK);
}

CloudStatus UserValidator::retrieve(const CloudConfig& config, User& user)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    return CloudStatus(CLOUD_OK);
}

CloudStatus UserValidator::update(const CloudConfig& config, const User& user)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    return CloudStatus(CLOUD_OK);
}

CloudStatus UserValidator::reqLoginCode(const CloudConfig& config,
                                        const std::string& sOrgKey,
                                        const std::string& sPhoNum)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(sOrgKey);
    MACRO_RETURN_ERROR_IF_EMPTY(sPhoNum);
    return CloudStatus(CLOUD_OK);
}

CloudStatus UserValidator::loginWithPhoneCode(CloudConfig& config,
                                              const std::string& sOrgKey,
                                              const std::string& sPhoNum,
                                              const std::string& sPhoCode)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(sOrgKey);
    MACRO_RETURN_ERROR_IF_EMPTY(sPhoNum);
    MACRO_RETURN_ERROR_IF_EMPTY(sPhoCode);
    return CloudStatus(CLOUD_OK);
}

CloudStatus UserValidator::retrieveUserRole(const CloudConfig& config, UserRole& userRole)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    return CloudStatus(CLOUD_OK);
}

CloudStatus UserValidator::sendPasswordReset(const CloudConfig& config,
                                             const std::string& sIdentifier,
                                             std::string& sResetToken)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(sIdentifier);
    return CloudStatus(CLOUD_OK);
}

CloudStatus UserValidator::resetPassword(const CloudConfig& config,
                                         const std::string& sPassword,
                                         const std::string& sResetToken)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(sPassword);
    MACRO_RETURN_ERROR_IF_EMPTY(sResetToken);
    return CloudStatus(CLOUD_OK);
}

CloudStatus UserValidator::sendVerificationCode(const CloudConfig& config,
                                                const std::string& sUserID,
                                                const std::string& sOrgKey)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(sUserID);
    MACRO_RETURN_ERROR_IF_EMPTY(sOrgKey);
    return CloudStatus(CLOUD_OK);
}

CloudStatus UserValidator::verifyAccount(const CloudConfig& config,
                                         const std::string& sUserID,
                                         const std::string& sVerCode)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(sUserID);
    MACRO_RETURN_ERROR_IF_EMPTY(sVerCode);
    return CloudStatus(CLOUD_OK);
}
