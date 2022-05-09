// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/cli/UserCommand.hpp"
#include "dfx/api/UserAPI.hpp"
#include "nlohmann/json.hpp"

using dfx::api::CloudStatus;
using dfx::api::User;
using nlohmann::json;

UserGetCommand::UserGetCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options)), operateOnSelf(false)
{
    cmd = createGetCommand(get, "user");
    cmd->add_flag("-s,--self", operateOnSelf, "Operate on self");
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

    if (operateOnSelf && ids.size() > 0) {
        CloudStatus status(CLOUD_PARAMETER_VALIDATION_ERROR, "No ids are allowed when operating on self");
        outputError(status);
        return DFXExitCode::FAILURE;
    }

    // If specified on command line, override the config limit
    if (cmd->count("--limit") > 0) {
        config.listLimit = limit;
    }

    int16_t totalCount;
    std::vector<User> users;
    User self;
    CloudStatus status(CLOUD_OK);
    if (operateOnSelf) {
        status = user->retrieve(config, self);
    } else {
        if (ids.size() == 0) {
            status = user->list(config, {}, offset, users, totalCount);
        } else {
            for (auto id : ids) {
                User u;
                std::string email(""); // How best to specify on CLI?
                status = user->retrieve(config, id, email, u);
                if (status.OK()) {
                    users.push_back(u);
                }
            }
        }
    }

    if (status.code == CLOUD_OK) {
        if (operateOnSelf) {
            outputResponse({{"user", self}, {"total", 1}});
        } else {
            outputResponse({{"users", users}, {"total", totalCount}});
        }
        return DFXExitCode::SUCCESS;
    } else {
        outputError(status);
        return DFXExitCode::FAILURE;
    }
}