// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/cli/StudyCommand.hpp"
#include "dfx/api/StudyAPI.hpp"
#include "dfx/api/utils/FileUtils.hpp"
#include "fmt/format.h"
#include "nlohmann/json.hpp"

using dfx::api::CloudStatus;
using dfx::api::Study;
using nlohmann::json;

StudyGetCommand::StudyGetCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options))
{
    cmd = createGetCommand(get, "study");
    cmd->add_option("-s,--study", studyConfigFile, "Name file to save study config");
    cmd->callback([&]() { result = cleanup(setupAndExecute()); });
}

DFXExitCode StudyGetCommand::execute()
{
    auto study = client->study(config);
    if (!study) {
        CloudStatus status(CLOUD_UNSUPPORTED_TRANSPORT, "Study implementation unavailable");
        outputError(status);
        return DFXExitCode::FAILURE;
    }

    if (!studyConfigFile.empty() && ids.size() != 1) {
        CloudStatus status(CLOUD_PARAMETER_VALIDATION_ERROR, "Study config only available with one ID");
        outputError(status);
        return DFXExitCode::FAILURE;
    }

    // If specified on command line, override the config limit
    if (cmd->count("--limit") > 0) {
        config.listLimit = limit;
    }

    int16_t totalCount;
    std::vector<Study> studies;
    CloudStatus status(CLOUD_OK);
    if (ids.size() == 0) {
        status = study->list(config, {}, offset, studies, totalCount);
    } else {
        status = study->retrieveMultiple(config, ids, studies);
    }

    std::string hashID;
    if (studies.size() == 1 && !studyConfigFile.empty()) {
        const std::string studyID(studies[0].id);
        const std::string sdkID;
        const std::string currentHashID;
        std::vector<uint8_t> studyData;
        auto status = study->retrieveStudyConfig(config, studyID, sdkID, currentHashID, studyData, hashID);
        if (status.OK()) {
            if (!dfx::api::utils::writeFile(studyConfigFile, studyData)) {
                std::cerr << "Unable to save study config file: " << studyConfigFile << " with " << studyData.size()
                          << " bytes." << std::endl;
            }
        } else {
            std::cerr << status << std::endl;
        }
    }

    if (status.code == CLOUD_OK) {
        if (hashID.empty()) {
            outputResponse({{"studies", studies}, {"total", totalCount}});
        } else {
            outputResponse({{"studies", studies}, {"studyhash", hashID}, {"total", totalCount}});
        }
        return DFXExitCode::SUCCESS;
    } else {
        outputError(status);
        return DFXExitCode::FAILURE;
    }
}