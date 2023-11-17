// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#ifndef DFXAPI_CLOUDTESTS_HPP
#define DFXAPI_CLOUDTESTS_HPP

#include "dfx/api/CloudAPI.hpp"

#include "gflags/gflags.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "fmt/format.h"
#include "nlohmann/json.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////
// Google CloudTests Fixture to handle authentication setup/teardown for all tests (except
// CloudAPI which explicitly tests these operations).
//   Setup()
//      1. Loads configuration into config
//      2. Register device to get config.deviceToken (if not provided)
//      3. Login to upgrade device token in config.authToken (if not provided)
//   Teardown()
//      1. Logout and clear config.authToken (if not provided)
//      2. Unregister and clear config.deviceToken (if not provided)
///////////////////////////////////////////////////////////////////////////////////////////////
namespace dfx
{
namespace api
{
namespace tests
{
class CloudTests : public ::testing::Test
{
protected:
    CloudTests();

    ~CloudTests() override;

    void SetUp() override;

    void TearDown() override;

    std::string getTestStudyID(const CloudConfig& config);

    std::shared_ptr<dfx::api::CloudAPI> client;
    dfx::api::CloudConfig config;
    std::ostream output;

    // These two flags are to allow the user to use pre-existing tokens and skip the login/logout in
    // setup/teardown.
    bool fixtureDidRegisterDevice = false; // If fixture registers device, it needs to release
    bool fixtureDidLogin = false;          // If fixture logins, it needs to release
};
} // namespace tests
} // namespace api
} // namespace dfx

#endif // DFXAPI_CLOUDTESTS_HPP
