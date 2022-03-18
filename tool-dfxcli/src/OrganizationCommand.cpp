// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/cli/OrganizationCommand.hpp"
#include "dfx/api/OrganizationAPI.hpp"
#include "nlohmann/json.hpp"

using dfx::api::CloudStatus;
using dfx::api::Organization;
using nlohmann::json;

OrganizationGetCommand::OrganizationGetCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options))
{
    cmd = createGetCommand(get, "organization");
    cmd->alias("org");
    cmd->callback([&]() { result = cleanup(setupAndExecute()); });
}

DFXExitCode OrganizationGetCommand::execute()
{
    auto org = client->organization(config);
    if (!org) {
        CloudStatus status(CLOUD_UNSUPPORTED_TRANSPORT, "Organization implementation unavailable");
        outputError(status);
        return DFXExitCode::FAILURE;
    }

    // If specified on command line, override the config limit
    if (cmd->count("--limit") > 0) {
        config.listLimit = limit;
    }

    int16_t totalCount;
    std::vector<Organization> organizations;
    CloudStatus status(CLOUD_OK);
    if (ids.size() == 0) {
        status = org->list(config, {}, offset, organizations, totalCount);
    } else {
        status = org->retrieveMultiple(config, ids, organizations);
    }

    if (status.code == CLOUD_OK) {
        outputResponse({{"organizations", organizations}, {"total", totalCount}});
        return DFXExitCode::SUCCESS;
    } else {
        outputError(status);
        return DFXExitCode::FAILURE;
    }
}