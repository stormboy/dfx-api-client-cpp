
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
        GTEST_SKIP() << "User endpoint does not exist for transport: " + client->getTransportType();
    }

    int16_t totalCount;
    std::vector<User> users;
    auto status = service->list(config, {}, 0, users, totalCount);
    if (status.code == CLOUD_UNSUPPORTED_FEATURE) {
        // REST has no support
        ASSERT_EQ(client->getTransportType(), CloudAPI::TRANSPORT_TYPE_REST) << "Expected transport to have support";
        GTEST_SKIP() << "UserTests::ListUsers(): CLOUD_UNSUPPORTED_FEATURE";
    } else if (status.code == CLOUD_USER_NOT_AUTHORIZED) {
        GTEST_SKIP() << "UserTests::ListUsers(): CLOUD_USER_NOT_AUTHORIZED";
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

TEST_F(UserTests, RetrieveUserSelf)
{
    auto service = client->user(config);
    if (service == nullptr) {
        GTEST_SKIP() << "User endpoint does not exist for transport: " + client->getTransportType();
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

