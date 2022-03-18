// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#ifndef DFXAPI_DEVICECOMMAND_HPP
#define DFXAPI_DEVICECOMMAND_HPP

#include <string>
#include <vector>

#include "dfx/api/cli/DFXAppCommand.hpp"

class DeviceGetCommand : public DFXAppCommand
{
public:
    DeviceGetCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result);
    DFXExitCode execute() override;
};

class DeviceDeleteCommand : public DFXAppCommand
{
public:
    DeviceDeleteCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result);
    DFXExitCode execute() override;

private:
    std::vector<std::string> ids;
};

#endif // DFXAPI_DEVICECOMMAND_HPP
