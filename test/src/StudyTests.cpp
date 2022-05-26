// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/tests/CloudTests.hpp"

using namespace dfx::api;
using namespace dfx::api::tests;

class StudyTests : public CloudTests
{
protected:
    void SetUp() override
    {
        CloudTests::SetUp();

        studyID = config.studyID;
        ;
        sdkID = "4.3.13,3.2.2,Darwin,x86_64";
        currentHashID = "";
    }

    void TearDown() override { CloudTests::TearDown(); }

    std::string studyID;
    std::string sdkID;
    std::string currentHashID;
};

///////////////////////////////////////////////////////////////////////////////
// STUDY TESTS
///////////////////////////////////////////////////////////////////////////////
TEST_F(StudyTests, ListStudyTemplates)
{
    std::list<StudyTemplate> studyTemplates;
    auto status = client->study(config)->listStudyTemplates(config, StudyStatus::ACTIVE, "all", studyTemplates);
    if (status.code == CLOUD_USER_NOT_AUTHORIZED) {
        GTEST_SKIP() << "StudyTests::ListStudyTemplates(): USER_NOT_AUTHORIZED";
    }
    if (status.code == CLOUD_UNSUPPORTED_FEATURE) {
        GTEST_SKIP() << status;
    }

    // Temporarily, skip this for now, server is returning "INCORRECT_REQUEST" on WebSocket
    if ((client->getTransportType().compare(CloudAPI::TRANSPORT_TYPE_WEBSOCKET) == 0) &&
        (status.code == CLOUD_PARAMETER_VALIDATION_ERROR)) {
        GTEST_SKIP() << "StudyTests::ListStudyTemplates(): CLOUD_PARAMETER_VALIDATION_ERROR";
    }

    ASSERT_EQ(status.code, CLOUD_OK) << status;

    if (output) {
        output << "StudyTest::ListStudyTemplates(): (" << studyTemplates.size() << " records)\n";
        for (const auto& studyTemplate : studyTemplates) {
            output << "\t'" << studyTemplate.name << "','" << studyTemplate.description << "'\n";
        }
        output << std::endl;
    }
}

TEST_F(StudyTests, RetrieveActiveStudyTypes)
{
    std::list<StudyType> studyTypes;
    auto status = client->study(config)->retrieveStudyTypes(config, StudyStatus::ACTIVE, studyTypes);
    if (status.code == CLOUD_USER_NOT_AUTHORIZED) {
        GTEST_SKIP() << "StudyTests::RetrieveActiveStudyTypes(): USER_NOT_AUTHORIZED";
    }
    if (status.code == CLOUD_UNSUPPORTED_FEATURE) {
        GTEST_SKIP() << status;
    }
    ASSERT_EQ(status.code, CLOUD_OK) << status;

    if (output) {
        output << "StudyTest::RetrieveStudyTypes(ACTIVE): (" << studyTypes.size() << " records)\n";
        for (const auto& studyType : studyTypes) {
            output << "\t'" << studyType.id << "," << studyType.name << "','" << studyType.description << "'\n";
        }
        output << std::endl;
    }
}

TEST_F(StudyTests, Retrieve)
{
    int16_t totalCount;
    std::vector<Study> studies;
    auto status = client->study(config)->list(config, {}, 0, studies, totalCount);
    if (status.code == CLOUD_USER_NOT_AUTHORIZED) {
        GTEST_SKIP() << "StudyTests::list(): USER_NOT_AUTHORIZED";
    }
    ASSERT_EQ(status.code, CLOUD_OK) << status;

    for (const auto& item : studies) {
        Study study;
        // list() will likely have already done this retrieve call but test it explicitly
        status = client->study(config)->retrieve(config, item.id, study);
        if (status.code == CLOUD_USER_NOT_AUTHORIZED) {
            GTEST_SKIP() << "StudyTests::Retrieve(): USER_NOT_AUTHORIZED";
        }
        ASSERT_EQ(status.code, CLOUD_OK) << status;
        ASSERT_EQ(study.id.empty(), false) << "Study ID should not be empty";

        output << study.id << std::endl;
    }
}

TEST_F(StudyTests, ListStudies)
{
    int16_t totalCount;
    std::vector<Study> studies;
    auto status = client->study(config)->list(config, {}, 0, studies, totalCount);
    ASSERT_EQ(status.code, CLOUD_OK) << status;
    ASSERT_NE(studies.empty(), true) << "Server should have at least one study";
}

TEST_F(StudyTests, RetreiveStudyConfig)
{
    std::vector<uint8_t> studyData;
    std::string hashID;
    auto status = client->study(config)->retrieveStudyConfig(config, studyID, sdkID, currentHashID, studyData, hashID);
    if (status.code == CLOUD_UNSUPPORTED_FEATURE) {
        GTEST_SKIP() << status;
    }
    ASSERT_EQ(status.code, CLOUD_OK) << status;
    ASSERT_NE(studyData.empty(), true) << "Expected server to return study config bytes";
}
