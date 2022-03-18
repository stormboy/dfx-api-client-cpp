// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_ORGANIZATION_VALIDATOR_H
#define DFX_API_CLOUD_ORGANIZATION_VALIDATOR_H

#include "dfx/api/OrganizationAPI.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

namespace dfx::api::validator
{

class OrganizationValidator : public OrganizationAPI
{
public:
    OrganizationValidator() = default;

    ~OrganizationValidator() override = default;
    CloudStatus create(const CloudConfig& config,
                       const std::string& name,
                       const std::string& identifier,
                       const std::string& public_key,
                       const dfx::api::OrganizationStatus& status,
                       const std::string& logo,
                       std::string& organizationID) override;

    CloudStatus list(const CloudConfig& config,
                     const std::unordered_map<OrganizationFilter, std::string>& filters,
                     uint16_t offset,
                     std::vector<dfx::api::Organization>& organizationList,
                     int16_t& totalCount) override;

    CloudStatus retrieve(const CloudConfig& config,
                         const std::string& organizationID,
                         dfx::api::Organization& organization) override;

    CloudStatus retrieveMultiple(const CloudConfig& config,
                                 const std::vector<std::string>& organizationIDs,
                                 std::vector<dfx::api::Organization>& organizations) override;

    CloudStatus update(const CloudConfig& config, dfx::api::Organization& organization) override;

    CloudStatus remove(const CloudConfig& config, const std::string& organizationID) override;

    static const OrganizationValidator& instance();
};

} // namespace dfx::api::validator

#endif // DFX_API_CLOUD_ORGANIZATION_VALIDATOR_H
