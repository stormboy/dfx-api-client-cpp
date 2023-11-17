// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/validator/OrganizationValidator.hpp"
#include "CloudValidatorMacros.hpp"

using namespace dfx::api;
using namespace dfx::api::validator;

const OrganizationValidator& OrganizationValidator::instance()
{
    static const OrganizationValidator instance;
    return instance;
}

CloudStatus OrganizationValidator::create(const CloudConfig& config,
                                          const std::string& name,
                                          const std::string& identifier,
                                          const std::string& public_key,
                                          const dfx::api::OrganizationStatus& status,
                                          const std::string& logo,
                                          std::string& organizationID)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(name);
    MACRO_RETURN_ERROR_IF_EMPTY(identifier);
    MACRO_RETURN_ERROR_IF_EMPTY(public_key);
    MACRO_RETURN_ERROR_IF_EMPTY(logo);
    return CloudStatus(CLOUD_OK);
}

CloudStatus OrganizationValidator::list(const CloudConfig& config,
                                        const std::unordered_map<OrganizationFilter, std::string>& filters,
                                        uint16_t offset,
                                        std::vector<dfx::api::Organization>& organizations,
                                        int16_t& totalCount)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    return CloudStatus(CLOUD_OK);
}

CloudStatus OrganizationValidator::retrieve(const CloudConfig& config,
                                            const std::string& organizationID,
                                            dfx::api::Organization& organization)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(organizationID);
    return CloudStatus(CLOUD_OK);
}

CloudStatus OrganizationValidator::retrieveMultiple(const CloudConfig& config,
                                                    const std::vector<std::string>& organizationIDs,
                                                    std::vector<dfx::api::Organization>& organizations)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(organizationIDs);
    return CloudStatus(CLOUD_OK);
}

CloudStatus OrganizationValidator::update(const CloudConfig& config, dfx::api::Organization& organization)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    return CloudStatus(CLOUD_OK);
}

CloudStatus OrganizationValidator::remove(const CloudConfig& config, const std::string& organizationID)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(organizationID);
    return CloudStatus(CLOUD_OK);
}

CloudStatus OrganizationValidator::listUsers(const CloudConfig& config,
                                        const std::unordered_map<dfx::api::UserAPI::UserFilter, std::string>& filters,
                                        uint16_t offset,
                                        std::vector<User>& users,
                                        int16_t& totalCount) {
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    return CloudStatus(CLOUD_OK);
}

CloudStatus
OrganizationValidator::retrieveUser(const CloudConfig& config, const std::string& userID, const std::string& email, User& user)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(userID);
    MACRO_RETURN_ERROR_IF_EMPTY(email);
    return CloudStatus(CLOUD_OK);
}

CloudStatus
OrganizationValidator::updateUser(const CloudConfig& config, const std::string& userID, const std::string& email, const User& user)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(userID);
    MACRO_RETURN_ERROR_IF_EMPTY(email);
    return CloudStatus(CLOUD_OK);
}

CloudStatus OrganizationValidator::removeUser(const CloudConfig& config, const std::string& userID, const std::string& email)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(userID);
    MACRO_RETURN_ERROR_IF_EMPTY(email);
    return CloudStatus(CLOUD_OK);
}