// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#ifndef DFXAPI_AUTHCOMMANDS_HPP
#define DFXAPI_AUTHCOMMANDS_HPP

#include <string>
#include <vector>

#include "dfx/api/cli/DFXAppCommand.hpp"

class AuthLoginCommand : public DFXAppCommand
{
public:
    AuthLoginCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result);
    DFXExitCode execute() override;

private:
    bool registerDevice;
};

class AuthLogoutCommand : public DFXAppCommand
{
public:
    AuthLogoutCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result);
    DFXExitCode execute() override;

private:
    std::vector<std::string> authTokens;
};

class AuthDeviceRegisterCommand : public DFXAppCommand
{
public:
    AuthDeviceRegisterCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result);
    DFXExitCode execute() override;
};

class AuthDeviceUnregisterCommand : public DFXAppCommand
{
public:
    AuthDeviceUnregisterCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result);
    DFXExitCode execute() override;

private:
    std::vector<std::string> deviceTokens;
};

#endif // DFXAPI_AUTHCOMMANDS_HPP
