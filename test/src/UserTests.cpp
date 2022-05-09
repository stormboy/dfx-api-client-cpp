
// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/tests/CloudTests.hpp"
#include "fmt/format.h"

using namespace dfx::api;
using namespace dfx::api::tests;

class UserTests : public CloudTests
{
protected:
    void SetUp() override
    {
        auto random_num = (rand() % 10000) + 1; // between [1-10000]

        CloudTests::SetUp();
        user.email = fmt::format("testuser-{}@nuralogix.ai", random_num);
        user.password = "^8OlK6r7IXmz&73";
        if (config.transportType == CloudAPI::TRANSPORT_TYPE_GRPC) {
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
        user = User();
        CloudTests::TearDown();
    }

    User user;
};

///////////////////////////////////////////////////////////////////////////////
// USER TESTS
///////////////////////////////////////////////////////////////////////////////

TEST_F(UserTests, ListUsers)
{
    auto service = client->user(config);
    if (service == nullptr) {
        GTEST_SKIP() << "User endpoint does not exist for transport: " + config.transportType;
    }

    int16_t totalCount;
    std::vector<User> users;
    auto status = service->list(config, {}, 0, users, totalCount);
    if (status.code == CLOUD_UNSUPPORTED_FEATURE) {
        // REST has no support
        ASSERT_EQ(config.transportType, CloudAPI::TRANSPORT_TYPE_REST) << "Expected transport to have support";
        GTEST_SKIP() << "UserTests::ListUsers(): CLOUD_UNSUPPORTED_FEATURE";
    } else {
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        if (totalCount != 0) {
            ASSERT_LE(users.size(), totalCount) << "Not enough users received";
        }

        if (output) {
            output << "UserTests::ListUsers(): (" << users.size() << " records of " << totalCount << " total)\n";
            for (const auto& user : users) {
                output << "\t'" << user.id << "','"
                       << "','" << user.firstName << "','" << user.role << "'\n";
            }
            output << std::endl;
        }
    }
}

TEST_F(UserTests, ListRetrieveUserAsAdmin)
{
    auto service = client->user(config);
    if (service == nullptr) {
        GTEST_SKIP() << "User endpoint does not exist for transport: " + config.transportType;
    }

    int16_t totalCount;
    std::vector<User> users;
    auto status = service->list(config, {}, 0, users, totalCount);
    if (status.code == CLOUD_UNSUPPORTED_FEATURE) {
        // REST has no support
        ASSERT_EQ(config.transportType, CloudAPI::TRANSPORT_TYPE_REST) << "Expected transport to have support";
        GTEST_SKIP() << "UserTests::ListUsers(): CLOUD_UNSUPPORTED_FEATURE";
    } else {
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        if (totalCount > 0) {
            ASSERT_LE(users.size(), totalCount) << "Not enough users received";

            const int MAX_USERS = 2; // Cap the number of users we will retrieve
            int users_retrieved = 0;
            for (const auto user : users) {
                if (users_retrieved++ > MAX_USERS) {
                    break;
                }

                User u;
                status = service->retrieve(config, user.id, user.email, u);
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

TEST_F(UserTests, RetrieveUserSelf)
{
    auto service = client->user(config);
    if (service == nullptr) {
        GTEST_SKIP() << "User endpoint does not exist for transport: " + config.transportType;
    }

    User self;
    auto status = service->retrieve(config, self);
    ASSERT_EQ(status.code, CLOUD_OK) << status;
    if (status.OK()) {
        if (output) {
            output << "UserTests::RetrieveUserSelf():\n";
            output << "ID: " << self.id << "\n";
            output << "Email: " << self.email << "\n";
            output << "Role: " << self.role << "\n";
            output << "First Name: " << self.firstName << "\n";
            output << "Last Name: " << self.lastName << "\n";
        }
    }
}

TEST_F(UserTests, CreateUpdateRemoveUser)
{
    if (config.transportType.compare(CloudAPI::TRANSPORT_TYPE_WEBSOCKET) == 0) {
        GTEST_SKIP() << "WebSocket Users does not allow admin management (Organization) functionality";
    }

    auto service = client->user(config);
    if (service == nullptr) {
        GTEST_SKIP() << "User endpoint does not exist for transport: " + config.transportType;
    }

    std::string userID;
    auto status = service->create(config,
                                  user.email,
                                  user.password,
                                  user.role,
                                  user.firstName,
                                  user.lastName,
                                  user.phoneNumber,
                                  user.gender,
                                  user.dateOfBirth,
                                  user.heightCM,
                                  user.weightKG,
                                  userID);

    if (status.code == CLOUD_USER_NOT_AUTHORIZED) {
        if (output) {
            output << "UserTests::CreateUpdateDeleteUser():\n";
            output << "\tCreate()  CLOUD_USER_NOT_AUTHORIZED - Test SKIPPED\n";
        }
        GTEST_SKIP() << "CLOUD_USER_NOT_AUTHORIZED - Test SKIPPED";
    } else if (status.code == CLOUD_RECORD_ALREADY_EXISTS) {
        if (output) {
            output << "UserTests::CreateUpdateDeleteUser():\n";
            output << "\tCreate()  CLOUD_RECORD_ALREADY_EXISTS - " << status.message << "\n";
        }
        // Test would have failed this time, lets try and clear it up for next time
        status = service->remove(config, user.id, user.email);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
    } else {
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        ASSERT_GT(userID.size(), 0) << "Created User ID must not be empty";
        if (output) {
            output << "UserTests::CreateUpdateDeleteUser():\n";
            output << "\tCreated: '" << userID << "','" << user.firstName << "'\n";
        }

        user.id = userID;
        user.firstName = "UpdatedTest";
        status = service->update(config, user);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        if (output) {
            output << "\tUpdated: '" << user.id << "','" << user.firstName << "'\n";
        }

        status = service->remove(config, user.id, user.email);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        if (output) {
            output << "\tRemoved: '" << user.id << "','" << user.firstName << "','" << user.email << "'\n";
            output << std::endl;
        }
    }
}
