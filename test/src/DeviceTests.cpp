// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/tests/CloudTests.hpp"

using namespace dfx::api;
using namespace dfx::api::tests;

class DeviceTests : public CloudTests
{
protected:
    void SetUp() override
    {
        CloudTests::SetUp();
        device.name = "TestDevice";
        device.type = DeviceType::LINUX;
        device.identifier = "DFX-LIVE";
        device.version = "2.0";
    }

    void TearDown() override
    {
        device = Device();
        CloudTests::TearDown();
    }

    Device device;
};

///////////////////////////////////////////////////////////////////////////////
// DEVICE TESTS
///////////////////////////////////////////////////////////////////////////////

TEST_F(DeviceTests, ListDevices)
{
    auto service = client->device(config);
    if (service == nullptr) {
        GTEST_SKIP() << "Device endpoint does not exist for transport: " + config.transportType;
    }

    int16_t totalCount;
    std::vector<Device> devices;
    auto status = service->list(config, {}, 0, devices, totalCount);
    if (status.code == CLOUD_USER_NOT_AUTHORIZED) {
        // Websocket user fails with RESTRICTED ACCESS on this test
        if (output) {
            output << "DeviceTest::ListDevices(): User was not authorized --> Test Ignored\n";
        }
    } else {
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        if (totalCount != 0) {
            // Inconsistent API WebSockets protobuf has no count, REST does
            ASSERT_LE(devices.size(), totalCount) << "Not enough devices received";
        }

        if (output) {
            output << "DeviceTest::ListDevices(): (" << devices.size() << " records of " << totalCount << " total)\n";
            for (const auto& device : devices) {
                output << "\t'" << device.name << "','"
                       << "','" << device.identifier << "','" << device.numberMeasurements << "'\n";
            }
            output << std::endl;
        }
    }
}

TEST_F(DeviceTests, CreateUpdateRemoveDevice)
{
    auto service = client->device(config);
    if (service == nullptr) {
        GTEST_SKIP() << "Device endpoint does not exist for transport: " + config.transportType;
    }
    if (config.transportType.compare(CloudAPI::TRANSPORT_TYPE_GRPC) == 0) {
        GTEST_SKIP() << "Device create does not exist for GRPC transport";
    }

    auto status = service->create(config, device.name, device.type, device.identifier, device.version, device);
    if (status.code == CLOUD_USER_NOT_AUTHORIZED) {
        if (output) {
            output << "DeviceTest::CreateUpdateDeleteDevice():\n";
            output << "\tCreate()  CLOUD_USER_NOT_AUTHORIZED - Test SKIPPED\n";
        }
        GTEST_SKIP() << "Device create CLOUD_USER_NOT_AUTHORIZED";
    } else {
        ASSERT_EQ(status.code, CLOUD_OK) << status;

        // If the next line asserts on WebSocket, there is a pending server patch to fix this
        ASSERT_GT(device.id.size(), 0) << "Created Device ID must not be empty";
        if (output) {
            output << "DeviceTest::CreateUpdateDeleteDevice():\n";
            output << "\tCreated: '" << device.id << "','" << device.name << "'\n";
        }

        device.name = "UpdatedTestDevice";
        status = service->update(config, device);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        if (output) {
            output << "\tUpdated: '" << device.id << "','" << device.name << "'\n";
        }

        status = service->remove(config, device.id);
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        if (output) {
            output << "\tRemoved: '" << device.id << "','" << device.name << "'\n";
            output << std::endl;
        }
    }
}