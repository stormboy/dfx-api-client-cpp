// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/tests/CloudTests.hpp"

using namespace dfx::api;
using namespace dfx::api::tests;

class ProfileTests : public CloudTests
{
protected:
    void SetUp() override
    {
        CloudTests::SetUp();

        profile.id = "640546c5-5da1-4163-a395-92684460b16b";
        profile.name = "TestProfile2";
        profile.email = "test-profile@nuralogix.ai";
        profile.status = ProfileStatus::ACTIVE;
    }

    void TearDown() override
    {
        profile = Profile();
        CloudTests::TearDown();
    }

    Profile profile;
};

///////////////////////////////////////////////////////////////////////////////
// PROFILE TESTS
///////////////////////////////////////////////////////////////////////////////

TEST_F(ProfileTests, listProfile)
{
    if (output) {
        output << "ProfileTest::listProfile():\n";
    }

    std::vector<Profile> profiles;
    int16_t totalCount;
    for (auto profileStatus : ProfileStatusMapper::getValues()) {
        auto statusString = ProfileStatusMapper::getString(profileStatus);
        if (output) {
            output << "\tProfile Status: " << statusString << "\n";
        }

        std::unordered_map<ProfileAPI::ProfileFilter, std::string> filters;
        filters.emplace(ProfileAPI::ProfileFilter::ProfileName, profile.name);

        // WebSocket does not support a status filter
        if (client->getTransportType().compare(CloudAPI::TRANSPORT_TYPE_WEBSOCKET) != 0) {
            filters.emplace(ProfileAPI::ProfileFilter::ProfileStatus, statusString);
        }

        auto service = client->profile(config);
        if (service == nullptr) {
            GTEST_SKIP() << "Profile endpoint does not exist for transport: " + client->getTransportType();
        }

        auto status = service->list(config, filters, 0, profiles, totalCount);
        if (status.code == CLOUD_USER_NOT_AUTHORIZED) {
            GTEST_SKIP() << "ProfileTests::listProfile(): USER_NOT_AUTHORIZED";
        }
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        for (const auto& record : profiles) {
            ASSERT_EQ(record.name, profile.name) << "Profile name should match queried profile name";
            if (output) {
                output << "\t\tProfile: '" << record.id << "','" << record.name << "','" << record.email << "'\n";
            }
        }
    }

    if (output) {
        output << std::endl;
    }
}

TEST_F(ProfileTests, CreateUpdateDeleteProfile)
{
    if (output) {
        output << "ProfileTest::CreateUpdateDeleteProfile():\n";
    }

    auto service = client->profile(config);
    if (service == nullptr) {
        GTEST_SKIP() << "Profile endpoint does not exist for transport: " + config.transportType;
    }

    // First, try and obtain the profile in case a previous test has left it lying around and remove it
    // if it does exist.
    auto status = service->retrieve(config, profile.id, profile);
    if (status.code == CLOUD_USER_NOT_AUTHORIZED) {
        GTEST_SKIP() << "ProfileTests::CreateUpdateDeleteProfile(): USER_NOT_AUTHORIZED";
    }
    if (status.code == CLOUD_OK) {
        if (output) {
            output << "\tRetrieved existing: '" << profile.id << "','" << profile.name << "','" << profile.email
                   << "'\n";
        }
        status = client->profile(config)->remove(config, profile.id);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        if (output) {
            output << "\tRemoved existing: '" << profile.id << "','" << profile.name << "','" << profile.email << "'\n";
        }
    } else {
        // If not successfully, retrieved must be not found
        ASSERT_EQ(status.code, CLOUD_RECORD_NOT_FOUND) << status;
        if (output) {
            output << "\tEnsured not existing: '" << profile.id << "','" << profile.name << "','" << profile.email
                   << "'\n";
        }
    }

    // Regular CRUD cycle now that we know the database does not have our previous attempt
    status = client->profile(config)->create(config, profile.name, profile.email, profile);
    ASSERT_EQ(status.code, CLOUD_OK) << status;
    if (output) {
        output << "\tCreated: '" << profile.id << "','" << profile.name << "','" << profile.email << "','"
               << ProfileStatusMapper::getString(profile.status) << "'\n";
    }

    profile.status = ProfileStatus::INACTIVE;
    status = client->profile(config)->update(config, profile);
    ASSERT_EQ(status.code, CLOUD_OK) << status;
    if (output) {
        output << "\tUpdated: '" << profile.id << "','" << profile.name << "','" << profile.email << "','"
               << ProfileStatusMapper::getString(profile.status) << "'\n";
    }

    status = client->profile(config)->remove(config, profile.id);
    ASSERT_EQ(status.code, CLOUD_OK) << status;
    if (output) {
        output << "\tRemoved: '" << profile.id << "','" << profile.name << "','" << profile.email << "','"
               << ProfileStatusMapper::getString(profile.status) << "'\n";
        output << std::endl;
    }
}

TEST_F(ProfileTests, ProfileNotFound)
{
    // Arbitrary profile ID, odds are good it should not exist.
    auto profileID = "640546c5-5da1-4163-a395-92684460b16b";

    auto service = client->profile(config);
    if (service == nullptr) {
        GTEST_SKIP() << "Profile endpoint does not exist for transport: " + client->getTransportType();
    }

    Profile profile;
    auto status = service->retrieve(config, profileID, profile);
    ASSERT_EQ(status.code, CLOUD_RECORD_NOT_FOUND) << status;
}
