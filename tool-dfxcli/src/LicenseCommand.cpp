// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/cli/LicenseCommand.hpp"
#include "dfx/api/LicenseAPI.hpp"
#include "nlohmann/json.hpp"

using dfx::api::CloudStatus;
using dfx::api::License;
using dfx::api::LicenseAPI;
using nlohmann::json;

LicenseGetCommand::LicenseGetCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options))
{
    cmd = createGetCommand(get, "license", false);
    cmd->callback([&]() { result = cleanup(setupAndExecute()); });
}

DFXExitCode LicenseGetCommand::execute()
{
    auto license = client->license(config);
    if (!license) {
        CloudStatus status(CLOUD_UNSUPPORTED_TRANSPORT, "License implementation unavailable");
        outputError(status);
        return DFXExitCode::FAILURE;
    }

    std::unordered_map<LicenseAPI::LicenseFilter, std::string> filters;
    if (cmd->count("--full") > 0) {
        filters.emplace(LicenseAPI::LicenseFilter::FullObject, "true");
    }

    int16_t totalCount;
    std::vector<dfx::api::License> licenses;
    auto status = license->list(config, {}, offset, licenses, totalCount);

    if (status.code == CLOUD_OK) {
        outputResponse({{"licenses", licenses}, {"total", totalCount}});
        return DFXExitCode::SUCCESS;
    } else {
        outputError(status);
        return DFXExitCode::FAILURE;
    }
}