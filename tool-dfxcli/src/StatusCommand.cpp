// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/cli/StatusCommand.hpp"
#include "nlohmann/json.hpp"

#include "dfx/api/CloudStatus.hpp"

using dfx::api::CloudStatus;
using nlohmann::json;

StatusCertificateCommand::StatusCertificateCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options))
{
    cmd = get->add_subcommand("rootca", "Fetch rootca from server");
    cmd->callback([&]() { result = cleanup(setupAndExecute()); });
}

DFXExitCode StatusCertificateCommand::execute()
{
    outputResponse({{"rootCA", config.rootCA}});
    return DFXExitCode::SUCCESS;
}

StatusConnectionCommand::StatusConnectionCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options))
{
    cmd = get->add_subcommand("connection", "Attempt to open a connection with server");
    cmd->callback([&]() { return DFXAppCommand::execute(connect(loadConfig())); });
}

DFXExitCode StatusConnectionCommand::execute()
{
    outputResponse({{"status", "connected"}});
    return DFXExitCode::SUCCESS;
}