// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#ifndef DFXAPI_STUDYCOMMAND_HPP
#define DFXAPI_STUDYCOMMAND_HPP

#include <string>
#include <vector>

#include "dfx/api/cli/DFXAppCommand.hpp"

class StudyGetCommand : public DFXAppCommand
{
public:
    StudyGetCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result);
    DFXExitCode execute() override;

    std::string studyConfigFile;
};

#endif // DFXAPI_STUDYCOMMAND_HPP
