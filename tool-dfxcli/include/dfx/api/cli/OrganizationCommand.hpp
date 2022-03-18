// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#ifndef DFXAPI_ORGANIZATIONCOMMAND_HPP
#define DFXAPI_ORGANIZATIONCOMMAND_HPP

#include <string>
#include <vector>

#include "dfx/api/cli/DFXAppCommand.hpp"

class OrganizationGetCommand : public DFXAppCommand
{
public:
    OrganizationGetCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result);
    DFXExitCode execute() override;
};

#endif // DFXAPI_ORGANIZATIONCOMMAND_HPP
