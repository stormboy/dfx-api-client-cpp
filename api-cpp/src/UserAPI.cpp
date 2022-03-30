// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/UserAPI.hpp"

using namespace dfx::api;

CloudStatus UserAPI::create(const CloudConfig& config,
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
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus UserAPI::list(const CloudConfig& config,
                          const std::unordered_map<UserFilter, std::string>& filters,
                          uint16_t offset,
                          std::vector<User>& users,
                          int16_t& totalCount)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus UserAPI::retrieve(const CloudConfig& config, User& user)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus UserAPI::update(const CloudConfig& config, const User& user)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus
UserAPI::retrieve(const CloudConfig& config, const std::string& userID, const std::string& email, User& user)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus
UserAPI::update(const CloudConfig& config, const std::string& userID, const std::string& email, const User& user)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus UserAPI::remove(const CloudConfig& config, const std::string& userID, const std::string& email)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus UserAPI::reqLoginCode(const CloudConfig& config, const std::string& sOrgKey, const std::string& sPhoNum)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus UserAPI::loginWithPhoneCode(CloudConfig& config,
                                        const std::string& sOrgKey,
                                        const std::string& sPhoNum,
                                        const std::string& sPhoCode)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus UserAPI::retrieveUserRole(const CloudConfig& config, UserRole& userRole)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus UserAPI::sendPasswordReset(const CloudConfig& config,
                                       const std::string& sIdentifier,
                                       std::string& sResetToken)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus UserAPI::resetPassword(const CloudConfig& config,
                                   const std::string& sPassword,
                                   const std::string& sResetToken)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus UserAPI::sendVerificationCode(const CloudConfig& config,
                                          const std::string& sUserID,
                                          const std::string& sOrgKey)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus UserAPI::verifyAccount(const CloudConfig& config, const std::string& sUserID, const std::string& sVerCode)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}
