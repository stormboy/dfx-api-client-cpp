// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#ifndef DFXAPI_STATUSCOMMAND_HPP
#define DFXAPI_STATUSCOMMAND_HPP

#include <string>
#include <vector>

#include "dfx/api/cli/DFXAppCommand.hpp"

class StatusCertificateCommand : public DFXAppCommand
{
public:
    StatusCertificateCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result);
    DFXExitCode execute() override;
};

class StatusConnectionCommand : public DFXAppCommand
{
public:
    StatusConnectionCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result);
    DFXExitCode execute() override;
};

#endif // DFXAPI_STATUSCOMMAND_HPP
