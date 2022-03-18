// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/OrganizationAPI.hpp"

using namespace dfx::api;

CloudStatus OrganizationAPI::create(const CloudConfig& config,
                                    const std::string& name,
                                    const std::string& identifier,
                                    const std::string& public_key,
                                    const OrganizationStatus& status,
                                    const std::string& logo,
                                    std::string& organizationID)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus OrganizationAPI::list(const CloudConfig& config,
                                  const std::unordered_map<OrganizationFilter, std::string>& filters,
                                  uint16_t offset,
                                  std::vector<Organization>& organizationList,
                                  int16_t& totalCount)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus OrganizationAPI::retrieve(const CloudConfig& config,
                                      const std::string& organizationID,
                                      Organization& organization)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

// Polyfill for web based on retrieve, gRPC it is natively implemented
CloudStatus OrganizationAPI::retrieveMultiple(const CloudConfig& config,
                                              const std::vector<std::string>& organizationIDs,
                                              std::vector<Organization>& organizations)
{
    // Validate will occur by first retrieve call

    std::vector<Organization> orgList;
    for (const auto& id : organizationIDs) {
        Organization org;
        auto status = retrieve(config, id, org);
        if (status.OK()) {
            orgList.push_back(org);
        } else {
            return status;
        }
    }

    // Copy all the items we retrieved - this ensures org state consistent on failure
    // and allows client to pass existing items in list without us clearing.
    organizations.insert(organizations.end(), orgList.begin(), orgList.end());
    return CloudStatus(CLOUD_OK);
}

CloudStatus OrganizationAPI::update(const CloudConfig& config, Organization& organization)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus OrganizationAPI::remove(const CloudConfig& config, const std::string& organizationID)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}
