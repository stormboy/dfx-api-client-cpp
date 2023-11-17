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

        auto random_num = (rand() % 10000) + 1; // between [1-10000]
        user.email = fmt::format("testuser-{}@nuralogix.ai", random_num);
        user.password = "^8OlK6r7IXmz&73";
        if (client->getTransportType().compare(CloudAPI::TRANSPORT_TYPE_GRPC) == 1) {
            user.role = "LEAD";
        } else {
            user.role = "DFX_LEAD";
        }
        user.firstName = "Test";
        user.lastName = "User";
        user.phoneNumber = "";
        user.gender = "male";
        user.dateOfBirth = "1970-01-01";
        user.heightCM = 163;
        user.weightKG = 70;
    }

    void TearDown() override
    {
        organization = Organization();
        user = User();
        CloudTests::TearDown();
    }

    Organization organization;
    User user;
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

TEST_F(OrganizationTests, ListRetrieveUserAsAdmin)
{
    auto service = client->organization(config);
    if (service == nullptr) {
        GTEST_SKIP() << "Organization endpoint does not exist for transport: " + client->getTransportType();
    }

    int16_t totalCount;
    std::vector<User> users;
    auto status = service->listUsers(config, {}, 0, users, totalCount);
    if (status.code == CLOUD_UNSUPPORTED_FEATURE) {
        // REST has no support
        ASSERT_EQ(client->getTransportType(), CloudAPI::TRANSPORT_TYPE_REST) << "Expected transport to have support";
        GTEST_SKIP() << "OrganizationTests::ListRetrieveUserAsAdmin(): CLOUD_UNSUPPORTED_FEATURE";
    } else if (status.code == CLOUD_USER_NOT_AUTHORIZED) {
        GTEST_SKIP() << "OrganizationTests::ListRetrieveUserAsAdmin(): CLOUD_USER_NOT_AUTHORIZED";
    } else {
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        if (totalCount > 0) {
            ASSERT_LE(users.size(), totalCount) << "Not enough users received";

            const int MAX_USERS = 2; // Cap the number of users we will retrieve
            int users_retrieved = 0;
            for (const auto user: users) {
                if (users_retrieved++ > MAX_USERS) {
                    break;
                }

                User u;
                status = service->retrieveUser(config, user.id, user.email, u);
                ASSERT_EQ(status.code, CLOUD_OK) << status;

                if (output) {
                    output << "User Retrieved: '" << u.id << "','"
                           << "','" << u.firstName << "','" << u.role << "'\n";
                }
            }
            if (output) {
                output << std::endl;
            }
        }
    }
}

TEST_F(OrganizationTests, CreateUpdateRemoveUser)
{
    auto service = client->organization(config);
    if (service == nullptr) {
        GTEST_SKIP() << "Organization endpoint does not exist for transport: " + client->getTransportType();
    }

    auto status = service->createUser(config,user);

    if (status.code == CLOUD_UNSUPPORTED_FEATURE) {
        if (output) {
            output << "UserTests::CreateUpdateRemoveUser():\n";
            output << "\tCreate()  CLOUD_USER_NOT_AUTHORIZED - Test SKIPPED\n";
        }
        GTEST_SKIP() << "CLOUD_UNSUPPORTED_FEATURE - Test SKIPPED";

    } else if (status.code == CLOUD_USER_NOT_AUTHORIZED) {
        if (output) {
            output << "UserTests::CreateUpdateRemoveUser():\n";
            output << "\tCreate()  CLOUD_USER_NOT_AUTHORIZED - Test SKIPPED\n";
        }
        GTEST_SKIP() << "CLOUD_USER_NOT_AUTHORIZED - Test SKIPPED";
    } else if (status.code == CLOUD_RECORD_ALREADY_EXISTS) {
        if (output) {
            output << "UserTests::CreateUpdateRemoveUser():\n";
            output << "\tCreate()  CLOUD_RECORD_ALREADY_EXISTS - " << status.message << "\n";
        }
        // Test would have failed this time, lets try and clear it up for next time
        status = service->removeUser(config, user.id, user.email);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
    } else {
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        ASSERT_GT(user.id.size(), 0) << "Created User ID must not be empty";
        if (output) {
            output << "UserTests::CreateUpdateDeleteUser():\n";
            output << "\tCreated: '" << user.id << "','" << user.firstName << "'\n";
        }

        user.firstName = "UpdatedTest";
        status = service->updateUser(config, user.id, user.email, user);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        if (output) {
            output << "\tUpdated: '" << user.id << "','" << user.firstName << "'\n";
        }

        status = service->removeUser(config, user.id, user.email);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        if (output) {
            output << "\tRemoved: '" << user.id << "','" << user.firstName << "','" << user.email << "'\n";
            output << std::endl;
        }
    }
}

TEST_F(OrganizationTests, ListUsers)
{
    auto service = client->organization(config);
    if (service == nullptr) {
        GTEST_SKIP() << "Organization endpoint does not exist for transport: " + client->getTransportType();
    }

    int16_t totalCount;
    std::vector<User> users;
    auto status = service->listUsers(config, {}, 0, users, totalCount);
    if (status.code == CLOUD_UNSUPPORTED_FEATURE) {
        GTEST_SKIP() << status;
    } else if (status.code == CLOUD_USER_NOT_AUTHORIZED) {
        GTEST_SKIP() << "OrganizationTests::ListUsers(): CLOUD_USER_NOT_AUTHORIZED";
    }

    ASSERT_EQ(status.code, CLOUD_OK) << status;
    ASSERT_LE(users.size(), std::min(static_cast<int16_t>(config.listLimit), totalCount)) << "Not enough users received";
    ASSERT_GE(users.size(), 1) << "Not enough users received";

    if (output) {
        output << "OrganizationTest::ListUsers(): (" << users.size() << " records of " << totalCount
               << " total)\n";
    }

    for (const auto& user : users) {
        User detail;
        status = service->retrieveUser(config, user.id, user.email, detail);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        ASSERT_EQ(user.id, detail.id) << "User ID's must match";

        if (output) {
            output << "\t'" << detail.id << "','" << detail.email << "','" << detail.firstName << "','" << detail.lastName
                   << "'\n";
        }
    }

    if (output) {
        output << std::endl;
    }
}
