// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/tests/CloudTests.hpp"

using namespace dfx::api;
using namespace dfx::api::tests;

class OrganizationTests : public CloudTests
{
protected:
    void SetUp() override
    {
        CloudTests::SetUp();
        organization.name = "Test";
        organization.identifier = "1234Test";
        organization.publicKey = "2308asdf32234r098";
        organization.status = OrganizationStatus::ACTIVE;
        organization.logo = "qa";
    }

    void TearDown() override
    {
        organization = Organization();
        CloudTests::TearDown();
    }

    Organization organization;
};

///////////////////////////////////////////////////////////////////////////////
// ORGANIZATION TESTS
///////////////////////////////////////////////////////////////////////////////

TEST_F(OrganizationTests, ListOrganizations)
{
    auto service = client->organization(config);
    if (service == nullptr) {
        GTEST_SKIP() << "Organization endpoint does not exist for transport: " + client->getTransportType();
    }

    int16_t totalCount;
    std::vector<Organization> organizations;
    auto status = service->list(config, {}, 0, organizations, totalCount);
    if (status.code == CLOUD_UNSUPPORTED_FEATURE) {
        GTEST_SKIP() << status;
    }

    ASSERT_EQ(status.code, CLOUD_OK) << status;
    ASSERT_LE(organizations.size(), totalCount) << "Not enough organizations received";
    ASSERT_GE(organizations.size(), 1) << "Not enough organizations received";

    if (output) {
        output << "OrganizationTest::ListOrganizations(): (" << organizations.size() << " records of " << totalCount
               << " total)\n";
    }

    for (const auto& organization : organizations) {
        Organization detail;
        status = service->retrieve(config, organization.id, detail);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        ASSERT_EQ(organization.id, detail.id) << "Organization ID's must match";

        if (output) {
            output << "\t'" << detail.id << "','" << detail.email << "','" << detail.identifier << "','" << detail.name
                   << "'\n";
        }
    }

    if (output) {
        output << std::endl;
    }
}

TEST_F(OrganizationTests, CreateRetrieveOrganization)
{
    auto service = client->organization(config);
    if (service == nullptr) {
        GTEST_SKIP() << "Organization endpoint does not exist for transport: " + config.transportType;
    }

    if (output) {
        output << "OrganizationTest::CreateRetrieveOrganization():";
    }

    std::string organizationID;
    auto status = service->create(config,
                                  organization.name,
                                  organization.identifier,
                                  organization.publicKey,
                                  organization.status,
                                  organization.logo,
                                  organizationID);
    if (status.code == CLOUD_UNSUPPORTED_FEATURE) {
        ASSERT_NE(client->getTransportType(), CloudAPI::TRANSPORT_TYPE_GRPC) << "gRPC has implementation";
        GTEST_SKIP() << "OrganizationTests::CreateRetrieveOrganization(): CLOUD_UNSUPPORTED_FEATURE";
    }

    ASSERT_EQ(status.code, CLOUD_OK) << status;

    if (output) {
        output << "\n\tCreated: '" << organizationID << "'\n";
    }

    Organization record;
    status = service->retrieve(config, organizationID, record);
    ASSERT_EQ(status.code, CLOUD_OK) << status;
    ASSERT_EQ(record.id, organizationID) << "Organization ID should match";
    ASSERT_EQ(record.name, organization.name) << "Organization Name should match";

    if (output) {
        output << "\n\tRetrieved: '" << organizationID << "','" << record.name << "','" << record.id << "'";
    }

    if (output) {
        output << "\n" << std::endl;
    }
}