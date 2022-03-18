// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/cli/SignalCommand.hpp"
#include "dfx/api/SignalAPI.hpp"
#include "nlohmann/json.hpp"

using dfx::api::CloudStatus;
using dfx::api::Signal;
using dfx::api::SignalAPI;
using nlohmann::json;

SignalGetCommand::SignalGetCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options))
{
    cmd = createGetCommand(get, "signal");
    cmd->callback([&]() { result = cleanup(setupAndExecute()); });
}

DFXExitCode SignalGetCommand::execute()
{
    auto signal = client->signal(config);
    if (!signal) {
        CloudStatus status(CLOUD_UNSUPPORTED_TRANSPORT, "Signal implementation unavailable");
        outputError(status);
        return DFXExitCode::FAILURE;
    }

    // If specified on command line, override the config limit
    if (cmd->count("--limit") > 0) {
        config.listLimit = limit;
    }

    std::unordered_map<SignalAPI::SignalFilter, std::string> filters;
    if (cmd->count("--full") > 0) {
        filters.emplace(SignalAPI::SignalFilter::FullObject, "true");
    }

    int16_t totalCount(0);
    std::vector<Signal> signals;
    CloudStatus status(CLOUD_OK);
    if (ids.size() == 0) {
        status = signal->list(config, filters, offset, signals, totalCount);
    } else {
        status = signal->retrieveMultiple(config, ids, signals);
    }

    if (status.code == CLOUD_OK) {
        outputResponse({{"signals", signals}, {"total", totalCount}});
        return DFXExitCode::SUCCESS;
    } else {
        outputError(status);
        return DFXExitCode::FAILURE;
    }
}