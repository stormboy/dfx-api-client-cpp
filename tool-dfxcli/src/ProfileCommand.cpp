// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/cli/ProfileCommand.hpp"
#include "dfx/api/ProfileAPI.hpp"
#include "nlohmann/json.hpp"

using dfx::api::CloudStatus;
using dfx::api::Profile;
using nlohmann::json;

ProfileGetCommand::ProfileGetCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options))
{
    cmd = createGetCommand(get, "profile");
    cmd->callback([&]() { result = cleanup(setupAndExecute()); });
}

DFXExitCode ProfileGetCommand::execute()
{
    auto profile = client->profile(config);
    if (!profile) {
        CloudStatus status(CLOUD_UNSUPPORTED_TRANSPORT, "Profile implementation unavailable");
        outputError(status);
        return DFXExitCode::FAILURE;
    }

    // If specified on command line, override the config limit
    if (cmd->count("--limit") > 0) {
        config.listLimit = limit;
    }

    int16_t totalCount;
    std::vector<Profile> profiles;
    auto status = profile->list(config, {}, offset, profiles, totalCount);

    if (status.code == CLOUD_OK) {
        outputResponse({{"profiles", profiles}, {"total", totalCount}});
        return DFXExitCode::SUCCESS;
    } else {
        outputError(status);
        return DFXExitCode::FAILURE;
    }
}