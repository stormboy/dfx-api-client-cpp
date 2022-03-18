// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include <CLI/CLI.hpp>

#include <csignal>

#include "dfx/api/cli/AuthCommands.hpp"
#include "dfx/api/cli/ConfigCommand.hpp"
#include "dfx/api/cli/DFXAppCommand.hpp"
#include "dfx/api/cli/DeviceCommand.hpp"
#include "dfx/api/cli/LicenseCommand.hpp"
#include "dfx/api/cli/MeasurementCommand.hpp"
#include "dfx/api/cli/OrganizationCommand.hpp"
#include "dfx/api/cli/ProfileCommand.hpp"
#include "dfx/api/cli/SignalCommand.hpp"
#include "dfx/api/cli/StatusCommand.hpp"
#include "dfx/api/cli/StudyCommand.hpp"
#include "dfx/api/cli/UserCommand.hpp"

void signalHandler(int sig)
{
    std::cerr << "Interrupt signal (" << sig << ") received. Running cleanup..." << std::endl;
    runCleanupOperations();

    signal(sig, SIG_DFL); // Activate default handling
    raise(sig);           // Re-raise
}

int main(int argc, char** argv)
{
    signal(SIGINT, signalHandler);
    signal(SIGABRT, signalHandler);

    auto options = std::make_shared<Options>(Options{"~/.dfxcloud.yaml", "", false, false, false});

    DFXExitCode result = DFXExitCode::SUCCESS;

    CLI::App app{"DFX server CLI tool"};
    app.require_subcommand();
    app.failure_message(CLI::FailureMessage::help);
    app.fallthrough(true);

    app.add_flag("--config", options->configFilePath, "Config file containing defaults")
        ->capture_default_str()
        ->expected(1)
        ->check(CLI::ExistingFile);
    app.add_option("-c,--context", options->context, "Config context to use");
    app.add_option("-v,--verbose", options->verbose, "Enable verbose logging");

    app.add_option("--username", options->username, "DFX Username")->group("Config File Overrides");
    app.add_option("--password", options->password, "DFX Password")->group("Config File Overrides");
    app.add_option("--org", options->organizationID, "DFX Organization")->group("Config File Overrides");
    app.add_option("--license", options->license, "DFX Organization License")->group("Config File Overrides");
    app.add_option("--host", options->host, "DFX Server Hostname")->group("Config File Overrides");
    app.add_option("--port", options->port, "DFX Server Port")->group("Config File Overrides");
    app.add_option("--timeout", options->timeout, "Timeout in milliseconds")->group("Config File Overrides");

    app.add_flag("-t,--table", options->table, "Output as text table")->group("Output Formatting");
    app.add_flag("-p,--pretty", options->pretty, "Output as JSON pretty formatted")->group("Output Formatting");

    AuthLoginCommand authLoginCommand(&app, options, result);
    AuthLogoutCommand authLogoutCommand(&app, options, result);
    AuthDeviceRegisterCommand authDeviceRegisterCommand(&app, options, result);
    AuthDeviceUnregisterCommand authDeviceUnregisterCommand(&app, options, result);

    auto create =
        app.add_subcommand("create", "Create an item or configuration")->group("Basic Commands")->require_subcommand(1);

    auto get =
        app.add_subcommand("get", "Retrieve item(s) or configuration")->group("Basic Commands")->require_subcommand(1);

    auto remove =
        app.add_subcommand("delete", "Delete item(s)")->group("Basic Commands")->require_subcommand(1)->alias("del");

    auto config =
        app.add_subcommand("config", "Show or create a sample config")->group("Other Commands")->require_subcommand(1);

    auto status = app.add_subcommand("status", "Check status of configuration/connection")
                      ->group("Other Commands")
                      ->require_subcommand(1);

    ConfigViewCommand configViewCommand(config, options, result);
    ConfigListCommand configListCommand(config, options, result);
    ConfigSampleCommand configSampleCommand(config, options, result);
    DeviceGetCommand deviceGetCommand(get, options, result);
    DeviceDeleteCommand deviceDeleteCommand(remove, options, result);
    LicenseGetCommand licenseGetCommand(get, options, result);
    MeasurementGetCommand measurementGetCommand(get, options, result);

    // These are aliases of each other "create measurement" and "process"
    MeasurementCreateCommand measurementCreateCommand(nullptr, create, options, result);
    MeasurementCreateCommand measurementProcessCommand(&app, nullptr, options, result);

    OrganizationGetCommand organizationGetCommand(get, options, result);
    ProfileGetCommand profileGetCommand(get, options, result);
    SignalGetCommand signalGetCommand(get, options, result);
    StudyGetCommand studyGetCommand(get, options, result);
    UserGetCommand userGetCommand(get, options, result);
    StatusCertificateCommand statusCertificateCommand(status, options, result);
    StatusConnectionCommand statusConnectionCommand(status, options, result);

    CLI11_PARSE(app, argc, argv);

    return static_cast<int>(result);
}
