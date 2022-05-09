// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_USER_VALIDATOR_H
#define DFX_API_CLOUD_USER_VALIDATOR_H

#include "dfx/api/UserAPI.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

namespace dfx::api::validator
{

class UserValidator : public UserAPI
{
public:
    UserValidator() = default;

    ~UserValidator() override = default;

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

    CloudStatus retrieve(const CloudConfig& config, User& user) override;

    CloudStatus update(const CloudConfig& config, const User& user) override;

    CloudStatus
    retrieve(const CloudConfig& config, const std::string& userID, const std::string& email, User& user) override;

    CloudStatus
    update(const CloudConfig& config, const std::string& userID, const std::string& email, const User& user) override;

    CloudStatus remove(const CloudConfig& config, const std::string& userID, const std::string& email) override;

    CloudStatus reqLoginCode(const CloudConfig& config,
                             const std::string& sOrgKey,
                             const std::string& sPhoNum) override;

    CloudStatus loginWithPhoneCode(CloudConfig& config,
                                   const std::string& sOrgKey,
                                   const std::string& sPhoNum,
                                   const std::string& sPhoCode) override;

    CloudStatus retrieveUserRole(const CloudConfig& config, UserRole& userRole) override;

    CloudStatus sendPasswordReset(const CloudConfig& config,
                                  const std::string& sIdentifier,
                                  std::string& sResetToken) override;

    CloudStatus resetPassword(const CloudConfig& config,
                              const std::string& sPassword,
                              const std::string& sResetToken) override;

    CloudStatus sendVerificationCode(const CloudConfig& config,
                                     const std::string& sUserID,
                                     const std::string& sOrgKey) override;

    CloudStatus verifyAccount(const CloudConfig& config,
                              const std::string& sUserID,
                              const std::string& sVerCode) override;

    static const UserValidator& instance();
};

} // namespace dfx::api::validator

#endif // DFX_API_CLOUD_USER_VALIDATOR_H
