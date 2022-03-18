// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_ORGANIZATION_REST_H
#define DFX_API_CLOUD_ORGANIZATION_REST_H

#include "dfx/api/OrganizationAPI.hpp"

namespace dfx::api::rest
{

class CloudREST;

class OrganizationREST : public OrganizationAPI
{
public:
    OrganizationREST() = default;

    ~OrganizationREST() override = default;

    CloudStatus create(const CloudConfig& config,
                       const std::string& name,
                       const std::string& identifier,
                       const std::string& public_key,
                       const OrganizationStatus& status,
                       const std::string& logo,
                       std::string& organizationID) override;

    CloudStatus list(const CloudConfig& config,
                     const std::unordered_map<OrganizationFilter, std::string>& filters,
                     uint16_t offset,
                     std::vector<Organization>& organizations,
                     int16_t& totalCount) override;

    CloudStatus retrieve(const CloudConfig& config,
                         const std::string& organizationID,
                         Organization& organization) override;

    CloudStatus update(const CloudConfig& config, Organization& organization) override;

    CloudStatus remove(const CloudConfig& config, const std::string& organizationID) override;

private:
    CloudStatus getLogo(const CloudConfig& config, const std::string& ID, std::string& logo);
};

} // namespace dfx::api::rest

#endif // DFX_API_CLOUD_ORGANIZATION_REST_H
