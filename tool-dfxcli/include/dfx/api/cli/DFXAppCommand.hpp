// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#ifndef DFXAPI_DFXAPPCOMMAND_HPP
#define DFXAPI_DFXAPPCOMMAND_HPP

#include "dfx/api/CloudAPI.hpp"

#include <CLI/CLI.hpp>

#include <iostream>
#include <memory>
#include <string>

struct Options
{
    std::string configFilePath;
    std::string context;
    uint8_t verbose;

    bool table;
    bool pretty;

    std::string username;
    std::string password;
    std::string host;
    uint16_t port;
    uint16_t timeout;
    std::string organizationID;
    std::string license;
};

enum class DFXExitCode : int
{
    SUCCESS = 0,
    FAILURE
};

class DFXAppCommand
{
public:
    explicit DFXAppCommand(std::shared_ptr<Options> options);

    void outputResponse(const nlohmann::json& json);
    void outputError(const nlohmann::json& json);
    void outputFormatted(std::ostream& os, const nlohmann::json& json);

    virtual DFXExitCode execute() = 0;

    DFXExitCode loadConfig();
    DFXExitCode connect(DFXExitCode code);
    DFXExitCode registerDevice(DFXExitCode);
    DFXExitCode login(DFXExitCode code);
    DFXExitCode execute(DFXExitCode code);

    DFXExitCode setupAndExecute();
    DFXExitCode cleanup(DFXExitCode code);

protected:
    CLI::App* cmd;
    std::shared_ptr<Options> options;

    dfx::api::CloudConfig config;
    std::shared_ptr<dfx::api::CloudAPI> client;

    // Common Get Flags
    CLI::App* createGetCommand(CLI::App* cmd, const std::string& name, bool includeIDs = true);
    std::vector<std::string> ids;
    bool full;
    uint16_t offset;
    uint16_t limit;
};

extern std::vector<std::function<void()>> callbacks;
void addCleanupOperation(std::function<void()> callback);
void runCleanupOperations();

#endif // DFXAPI_DFXAPPCOMMAND_HPP
