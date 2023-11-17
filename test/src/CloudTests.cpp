// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/tests/CloudTests.hpp"

#include <cstdlib> // for rand/srand
#include <ctime>   // for time
#include <thread>
#include <chrono>

DEFINE_string(config, "~/.dfxcloud.yaml", "Configuration file to use for connection details");
DEFINE_string(context, "", "Config context to use");

// Leaving on for now, but this should be false by default
DEFINE_bool(output, true, "Enable logging to standard out");

using namespace dfx::api;
using namespace dfx::api::tests;

CloudTests::CloudTests() : output(nullptr)
{
    srand((unsigned)time(0));

    config.contextID = FLAGS_context;

    // By default, we don't want a lot of logging so the nullptr rdbuf will make the output
    // stream bad. If the flag provided, we write to std::cout's buffer.
    if (FLAGS_output) {
        output.rdbuf(std::cout.rdbuf());
    }
}

CloudTests::~CloudTests() = default;

void CloudTests::SetUp()
{
    // Load once in the constructor to print out the details about the Cloud config
    auto status = dfx::api::loadCloudConfig(config, FLAGS_config);
    if (status.code != CLOUD_OK) {
        GTEST_SKIP() << "Unable to load cloud config: " << FLAGS_config;
    } else {
        if (config.serverHost.empty()) {
            GTEST_SKIP() << "Invalid context config: " << FLAGS_context;
        }
    }

    // Explicitly print to avoid writing out credentials in logs
    std::cout << "--------------------------------------------------------------------------\n";
    std::cout << "Testing with transport config type: " << config.transportType << "\n";
    std::cout << "\tserver=";
    if (config.transportType != CloudAPI::TRANSPORT_TYPE_GRPC) {
        std::cout << (config.secure ? "https://" : "http://");
    }
    std::cout << config.serverHost << ":" << config.serverPort << "\n";
    std::cout << "\tauth-email=" << config.authEmail << "\n";
    std::cout << "\tauth-org=" << config.authOrg << std::endl;

    status = dfx::api::CloudAPI::createInstance(config, client);
    if (status.OK()) {
        std::cout << "Instance Type: " << client->getTransportType() << "\n";
    }
    std::cout << "--------------------------------------------------------------------------\n";

    ASSERT_EQ(status.code, CLOUD_OK) << status;
    ASSERT_NE(client, nullptr) << "Cloud instance should not be null";

    if (config.deviceToken.empty()) {
        if (!config.license.empty()) {
            auto appName = std::string("APITest");
            auto appVersion = std::string("2.0");
            uint16_t tokenExpiresInSeconds = 3600;
            std::string tokenSubject = ""; // Do not lock the host used for communication
            auto status = client->registerDevice(config, appName, appVersion, tokenExpiresInSeconds, tokenSubject);
            ASSERT_NE(status.code, CLOUD_TRANSPORT_CLOSED)
                << "Connection establishment failure on transport: " << config.transportType;
            ASSERT_EQ(status.code, CLOUD_OK) << status;
            ASSERT_EQ(config.deviceToken.empty(), false) << "Device Token should not be empty";
            fixtureDidRegisterDevice = true;
        }
    }
    if (config.authToken.empty()) {
        if (!config.authPassword.empty()) {
            status = client->login(config);
            ASSERT_NE(status.code, CLOUD_TRANSPORT_CLOSED)
                << "Connection establishment failure on transport: " << config.transportType;
            ASSERT_EQ(status.code, CLOUD_OK) << status;
            ASSERT_EQ(config.authToken.empty(), false) << "Auth Token should not be empty";
            fixtureDidLogin = true;
        }
    }
}

void CloudTests::TearDown()
{
    if (fixtureDidLogin && !config.authToken.empty()) {
        auto status = client->logout(config);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        config.authToken = "";
        fixtureDidLogin = false;
    }

    if (fixtureDidRegisterDevice && !config.deviceToken.empty()) {
        // REST fails with RESTRICTED_ACCESS, skip if REST
        if (client->getTransportType().compare(CloudAPI::TRANSPORT_TYPE_REST) != 0) {
            auto status = client->unregisterDevice(config);
            ASSERT_EQ(status.code, CLOUD_OK) << status;
        }
        config.deviceToken = "";
        fixtureDidRegisterDevice = false;
    }
    client = nullptr;
}

std::string CloudTests::getTestStudyID(const CloudConfig& config)
{
    std::string studyID = config.studyID;
    if (studyID.empty()) {
        // Need a study ID, so use list to fetch the first we see
        std::vector<Study> studies;
        CloudConfig listConfig(config);
        listConfig.listLimit = 1;
        int16_t totalCount;
        auto status = client->study(config)->list(listConfig, {}, 0,
                                                  studies, totalCount);
        if ( status.OK() ) {
            studyID = studies[0].id;
        }
    }
    return studyID;
}

///////////////////////////////////////////////////////////////////////////////
// CloudAPI TESTS
///////////////////////////////////////////////////////////////////////////////


// Login performs essentially the same as the Setup/Teardown fixture, but is explicitly
// designed to just validate we can get the basics and avoid any pre-cached tokens in
// the config. If this test fails, there is a problem with the config and ALL other
// tests will fail so can be used for quick validation of config.
TEST(CloudAPI, login)
{
    CloudConfig config;
    auto status = dfx::api::loadCloudConfig(config, FLAGS_config);
    ASSERT_EQ(status.code, CLOUD_OK) << status;

    // We are testing the authentication cycle, clear the loaded tokens to test we are able
    // to register and login
    config.deviceToken = "";
    config.authToken = "";

    std::shared_ptr<CloudAPI> pClient;
    status = CloudAPI::createInstance(config, pClient);
    ASSERT_EQ(status.code, CLOUD_OK) << status;
    ASSERT_NE(pClient, nullptr) << "Cloud instance should not be null";

    auto appName = std::string("APITest");
    auto appVersion = std::string("2.0");
    uint16_t tokenExpiresInSeconds = 3600;
    std::string tokenSubject = "";
    status = pClient->registerDevice(config, appName, appVersion, tokenExpiresInSeconds, tokenSubject);
    ASSERT_EQ(status.code, CLOUD_OK) << status;
    ASSERT_EQ(config.deviceToken.empty(), false) << "Device Token should not be empty";

    if (!config.authPassword.empty()) {
        status = pClient->login(config);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        ASSERT_EQ(config.authToken.empty(), false) << "Auth Token should not be empty";

        status = pClient->logout(config);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
    }

    // REST fails with RESTRICTED_ACCESS, skip if REST
    // WebSocket fails also, so preform only if GRPC
    if (config.transportType.compare(CloudAPI::TRANSPORT_TYPE_GRPC) == 0) {
        status = pClient->unregisterDevice(config);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
    }
}

TEST(CloudAPI, verifyToken)
{
    CloudConfig config;
    auto status = dfx::api::loadCloudConfig(config, FLAGS_config);
    ASSERT_EQ(status.code, CLOUD_OK) << status;

    // We are testing the authentication cycle, clear the loaded tokens to test we are able
    // to register and login
    config.deviceToken = "";
    config.authToken = "";

    std::shared_ptr<CloudAPI> pClient;
    status = CloudAPI::createInstance(config, pClient);
    ASSERT_EQ(status.code, CLOUD_OK) << status;
    ASSERT_NE(pClient, nullptr) << "Cloud instance should not be null";

    auto appName = std::string("APITest");
    auto appVersion = std::string("2.0");
    uint16_t tokenExpiresInSeconds = 3600;
    std::string tokenSubject = "";
    status = pClient->registerDevice(config, appName, appVersion, tokenExpiresInSeconds, tokenSubject);
    ASSERT_EQ(status.code, CLOUD_OK) << status;
    ASSERT_EQ(config.deviceToken.empty(), false) << "Device Token should not be empty";

    std::string response;
    status = pClient->verifyToken(config, response);
    if (status.code == CLOUD_UNSUPPORTED_FEATURE) {
        GTEST_SKIP() << "verifyToken() not supported on transport: " << config.transportType;
    }
    ASSERT_EQ(status.code, CLOUD_OK) << status;

    // REST fails with RESTRICTED_ACCESS, skip if REST
    // WebSocket fails also, so preform only if GRPC
    if (config.transportType.compare(CloudAPI::TRANSPORT_TYPE_GRPC) == 0) {
        status = pClient->unregisterDevice(config);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
    }
}

TEST(CloudAPI, renewToken)
{
    CloudConfig config;
    auto status = dfx::api::loadCloudConfig(config, FLAGS_config);
    ASSERT_EQ(status.code, CLOUD_OK) << status;

    // We are testing the authentication cycle, clear the loaded tokens to test we are able
    // to register and login
    config.deviceToken = "";
    config.authToken = "";

    std::shared_ptr<CloudAPI> pClient;
    status = CloudAPI::createInstance(config, pClient);
    ASSERT_EQ(status.code, CLOUD_OK) << status;
    ASSERT_NE(pClient, nullptr) << "Cloud instance should not be null";

    auto appName = std::string("APITest");
    auto appVersion = std::string("2.0");
    uint16_t tokenExpiresInSeconds = 3600;
    std::string tokenSubject = "";
    status = pClient->registerDevice(config, appName, appVersion, tokenExpiresInSeconds, tokenSubject);
    ASSERT_EQ(status.code, CLOUD_OK) << status;
    ASSERT_EQ(config.deviceToken.empty(), false) << "Device Token should not be empty";

    // Sleep for 1 second to ensure the generate token time is different
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::string token = config.deviceToken;
    std::string refreshToken = config.deviceRefreshToken;
    status = pClient->renewToken(config, token, refreshToken);
    if (status.code == CLOUD_UNSUPPORTED_FEATURE) {
        GTEST_SKIP() << "verifyToken() not supported on transport: " << config.transportType;
    }
    ASSERT_EQ(status.code, CLOUD_OK) << status;
    ASSERT_NE(refreshToken, config.deviceRefreshToken) << "Refresh Token should have changed";
    ASSERT_NE(token, config.deviceToken) << "Token should have changed";

    // REST fails with RESTRICTED_ACCESS, skip if REST
    // WebSocket fails also, so preform only if GRPC
    if (config.transportType.compare(CloudAPI::TRANSPORT_TYPE_GRPC) == 0) {
        status = pClient->unregisterDevice(config);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
    }
}

// getAvailableTransports is to validate that we have compiled in the transport that
// we want in configuration.
TEST(CloudAPI, getAvailableTransports)
{
    CloudConfig config;
    auto status = dfx::api::loadCloudConfig(config, FLAGS_config);
    ASSERT_EQ(status.code, CLOUD_OK) << status;

    bool haveRequiredTransport = false;
    auto transports = CloudAPI::getAvailableTransports();
    ASSERT_NE(transports.size(), 0) << "Must have at least one transport available";

    if (config.transportType.empty()) {
        // If nothing explicitly requested, and we have at least one it will be used
        haveRequiredTransport = true;
    } else {
        for (const auto& transport : transports) {
            if (config.transportType.compare(transport) == 0) {
                haveRequiredTransport = true;
                break;
            }
        }
    }

    // This could happen because of a config typo, or the API was not built to have
    // the specific config.transportType enabled.
    ASSERT_EQ(haveRequiredTransport, true) << "Transport specified in config is unavailable";
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    return RUN_ALL_TESTS();
}
