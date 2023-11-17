// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/tests/CloudTests.hpp"

#include <chrono>
#include <fstream>
#include <thread>

using namespace dfx::api;
using namespace dfx::api::tests;

class MeasurementTests : public CloudTests
{
protected:
    void SetUp() override { CloudTests::SetUp(); }

    void TearDown() override
    {
        measurement = Measurement();
        CloudTests::TearDown();
    }

    Measurement measurement;
};

///////////////////////////////////////////////////////////////////////////////
// MEASUREMENT TESTS
///////////////////////////////////////////////////////////////////////////////

TEST_F(MeasurementTests, ListMeasurements)
{
    auto service = client->measurement(config);
    if (service == nullptr) {
        GTEST_SKIP() << "Measurement endpoint does not exist for transport: " + client->getTransportType();
    }

    int16_t totalCount;
    std::vector<Measurement> measurements;
    auto status = service->list(config, {}, true, measurements, totalCount);
    ASSERT_EQ(status.code, CLOUD_OK) << status;

    if (output) {
        output << "MeasurementTests::ListMeasurements(): (" << measurements.size() << ")\n";
        for (const auto& measurement : measurements) {
            output << "\t'" << measurement.id << "','" << measurement.studyID << "'\n";
        }
        output << std::endl;
    }
}

#include "dfx/api/utils/FileUtils.hpp"
#include <filesystem>
namespace fs = std::filesystem;

TEST_F(MeasurementTests, PerformMeasurement)
{
    // cmake-build-debug/test/unittest.exe -> ../../test_data/data == $REPO/test_data/data
    fs::path testData = fs::current_path().parent_path().parent_path() / "test_data" / "data";
    if (!fs::exists(testData)) {
        GTEST_SKIP() << "Test data folder ($REPO/test_data) is missing";
    }

    std::vector<std::filesystem::path> files;
    for (auto it = fs::directory_iterator(testData); it != fs::directory_iterator(); ++it) {
        files.push_back(it->path());
    }
    std::sort(files.begin(), files.end());

    std::shared_ptr<MeasurementStreamAPI> measurement = client->measurementStream(config);

    measurement->setResultCallback([this](const MeasurementResult& data) {
        if (output) {
            output << "Got Result  [" << data.timestampMS << "]:"
                   << "  Chunk=" << data.chunkOrder << ", FaceID=" << data.faceID;
            for (const auto& signal : data.signalData) {
                output << ", Signal=" << signal.first << ", Data=";
                size_t count = 0;
                const size_t last = signal.second.size();
                for (const auto& value : signal.second) {
                    output << value;
                    if (++count != last)
                        output << ",";
                }
            }
            output << std::endl;
        }
    });

    measurement->setMetricCallback([this](const MeasurementMetric& metric) {
        if (output) {
            output << "Got Metric: UploadRate=" << metric.uploadRate << std::endl;
        }
    });

    measurement->setWarningCallback([this](const MeasurementWarning& warning) {
        if (output) {
            output << "Got Warning [" << warning.timestampMS << "]:"
                   << "  Code=" << warning.warningCode << ", Message=" << warning.warningMessage << std::endl;
        }
    });

    std::string studyID = getTestStudyID(config);
    auto status = measurement->setupStream(config, studyID);
    if (status.code == CLOUD_UNSUPPORTED_FEATURE) {
        ASSERT_EQ(client->getTransportType(), CloudAPI::TRANSPORT_TYPE_REST) << "REST does not support streams";
        GTEST_SKIP() << status;
    }
    ASSERT_EQ(status.code, CLOUD_OK) << status;

    int count = 0;
    for (auto& file : files) {
        const std::vector<uint8_t> chunkData = dfx::api::utils::readFile(file);
        if (output) {
            output << "Read: " << file << " with " << chunkData.size() << " bytes."
                   << "\n";
        }

        bool isLast = ++count == files.size();
        status = measurement->sendChunk(config, chunkData, isLast);
        ASSERT_EQ(status.code, CLOUD_OK) << status;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (output) {
        output << "Measurement data fully sent, wait for completion\n";
    }

    status = measurement->waitForCompletion(config);

    if (output) {
        output << "Measurement Completion Status: " << status << "\n";
    }

    MeasurementResult result;

    while (measurement->getResult(result).OK()) {
        if (output) {
            output << "\tResult: \n";
            for (auto signal : result.signalData) {
                output << "\t\t" << signal.first << "=[ ";
                for (auto value : signal.second) {
                    output << value << " ";
                }
                output << "]\n";
            }
        }
    }
}
