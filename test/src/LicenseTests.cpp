// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/tests/CloudTests.hpp"

using namespace dfx::api;
using namespace dfx::api::tests;

class LicenseTests : public CloudTests
{
};

///////////////////////////////////////////////////////////////////////////////
// LICENSE TESTS
///////////////////////////////////////////////////////////////////////////////

TEST_F(LicenseTests, ListLicenses)
{
    auto service = client->license(config);
    if (service == nullptr) {
        GTEST_SKIP() << "License endpoint does not exist for transport: " + client->getTransportType();
    }

    int16_t totalCount;
    std::vector<License> licenses;
    auto status = service->list(config, {}, 0, licenses, totalCount);
    if (status.code == CLOUD_UNSUPPORTED_FEATURE || status.code == CLOUD_USER_NOT_AUTHORIZED) {
        GTEST_SKIP() << status;
    } else {
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        ASSERT_LE(licenses.size(), totalCount) << "Not enough licenses received";

        if (output) {
            output << "LicenseTest::ListLicenses(): (" << licenses.size() << " records of " << totalCount
                   << " total)\n";
            for (const auto& license : licenses) {
                output << "\t'" << license.id << "','" << license.status << "','" << license.licenseType << "','"
                       << license.key << "'\n";
            }
            output << std::endl;
        }
    }
}
