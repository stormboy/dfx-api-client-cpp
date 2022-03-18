// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#ifndef DFXAPI_CONFIGCOMMAND_HPP
#define DFXAPI_CONFIGCOMMAND_HPP

#include "dfx/api/cli/DFXAppCommand.hpp"

class ConfigViewCommand : public DFXAppCommand
{
public:
    ConfigViewCommand(CLI::App* config, std::shared_ptr<Options> options, DFXExitCode& result);
    DFXExitCode execute() override;

private:
    bool full;
};

class ConfigListCommand : public DFXAppCommand
{
public:
    ConfigListCommand(CLI::App* config, std::shared_ptr<Options> options, DFXExitCode& result);
    DFXExitCode execute() override;
};

class ConfigSampleCommand : public DFXAppCommand
{
public:
    ConfigSampleCommand(CLI::App* config, std::shared_ptr<Options> options, DFXExitCode& result);
    DFXExitCode execute() override;

private:
    bool advanced;
};

#endif // DFXAPI_CONFIGCOMMAND_HPP
