// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/cli/DeviceCommand.hpp"
#include "dfx/api/DeviceAPI.hpp"
#include "nlohmann/json.hpp"

using dfx::api::CloudStatus;
using dfx::api::Device;
using nlohmann::json;

DeviceGetCommand::DeviceGetCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options))
{
    cmd = createGetCommand(get, "device");
    cmd->callback([&]() { result = cleanup(setupAndExecute()); });
}

DFXExitCode DeviceGetCommand::execute()
{
    auto device = client->device(config);
    if (!device) {
        CloudStatus status(CLOUD_UNSUPPORTED_TRANSPORT, "Device implementation unavailable");
        outputError(status);
        return DFXExitCode::FAILURE;
    }

    // If specified on command line, override the config limit
    if (cmd->count("--limit") > 0) {
        config.listLimit = limit;
    }

    int16_t totalCount;
    std::vector<Device> devices;
    CloudStatus status(CLOUD_OK);
    if (ids.size() == 0) {
        status = device->list(config, {}, offset, devices, totalCount);
    } else {
        status = device->retrieveMultiple(config, ids, devices);
    }

    if (status.code == CLOUD_OK) {
        outputResponse({{"devices", devices}, {"total", totalCount}});
        return DFXExitCode::SUCCESS;
    } else {
        outputError(status);
        return DFXExitCode::FAILURE;
    }
}

DeviceDeleteCommand::DeviceDeleteCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options))
{
    cmd = get->add_subcommand("device", "Deletes a device");
    cmd->alias("dev");
    cmd->add_option("ids", ids, "Device IDs to delete")->required();
    cmd->callback([&]() { result = cleanup(setupAndExecute()); });
}

DFXExitCode DeviceDeleteCommand::execute()
{
    auto device = client->device(config);
    if (!device) {
        CloudStatus status(CLOUD_UNSUPPORTED_TRANSPORT, "Device implementation unavailable");
        outputError(status);
        return DFXExitCode::FAILURE;
    }

    std::vector<CloudStatus> failures;
    for (const auto& id : ids) {
        auto status = device->remove(config, id);
        if (status.code != CLOUD_OK) {
            status.message += " '" + id + "'";
            failures.push_back(status);
        }
    }

    if (failures.empty()) {
        return DFXExitCode::SUCCESS;
    } else {
        outputResponse(failures);
        return DFXExitCode::FAILURE;
    }
}
