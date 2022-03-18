// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/cli/UserCommand.hpp"
#include "dfx/api/UserAPI.hpp"
#include "nlohmann/json.hpp"

using dfx::api::CloudStatus;
using dfx::api::User;
using nlohmann::json;

UserGetCommand::UserGetCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options))
{
    cmd = createGetCommand(get, "user");
    cmd->callback([&]() { result = cleanup(setupAndExecute()); });
}

DFXExitCode UserGetCommand::execute()
{
    auto user = client->user(config);
    if (!user) {
        CloudStatus status(CLOUD_UNSUPPORTED_TRANSPORT, "User implementation unavailable");
        outputError(status);
        return DFXExitCode::FAILURE;
    }

    // If specified on command line, override the config limit
    if (cmd->count("--limit") > 0) {
        config.listLimit = limit;
    }

    int16_t totalCount;
    std::vector<User> users;
    auto status = user->list(config, {}, offset, users, totalCount);

    if (status.code == CLOUD_OK) {
        outputResponse({{"users", users}, {"total", totalCount}});
        return DFXExitCode::SUCCESS;
    } else {
        outputError(status);
        return DFXExitCode::FAILURE;
    }
}