// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/cli/ConfigCommand.hpp"
#include "nlohmann/json.hpp"

using nlohmann::json;

ConfigViewCommand::ConfigViewCommand(CLI::App* config, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options)), full(false)
{
    cmd = config->add_subcommand("view", "Display current configuration");
    cmd->alias("show");
    cmd->add_flag("-f,--full", full, "Show full details without obfuscation")->capture_default_str();
    cmd->callback([&]() { result = DFXAppCommand::execute(loadConfig()); });
}

DFXExitCode ConfigViewCommand::execute()
{
    json response = {{"context", config.contextID},
                     {"transport-type", config.transportType},
                     {"host", config.serverHost},
                     {"port", config.serverPort},
                     {"secure", config.secure},
                     {"skip-verify", config.skipVerify}};

    if (!config.authEmail.empty()) {
        response["auth-email"] = config.authEmail;
    }
    if (!config.authPassword.empty()) {
        response["auth-password"] = (full ? config.authPassword : "OBFUSCATED");
    }
    if (!config.authMFAToken.empty()) {
        response["auth-auth-mfa"] = (full ? config.authMFAToken : "OBFUSCATED");
    }
    if (!config.deviceRefreshToken.empty()) {
        response["device-refresh-token"] = (full ? config.deviceRefreshToken : "OBFUSCATED");
    }
    if (!config.userRefreshToken.empty()) {
        response["user-refresh-token"] = (full ? config.userRefreshToken : "OBFUSCATED");
    }
    if (!config.authOrg.empty()) {
        response["auth-org"] = config.authOrg;
    }
    if (!config.license.empty()) {
        response["license"] = (full ? config.license : "OBFUSCATED");
    }
    if (!config.studyID.empty()) {
        response["study-id"] = config.studyID;
    }
    if (!config.authToken.empty()) {
        response["auth-token"] = (full ? config.authToken : "OBFUSCATED");
    }
    if (!config.deviceToken.empty()) {
        response["device-token"] = (full ? config.deviceToken : "OBFUSCATED");
    }
    if (!config.rootCA.empty()) {
        response["root-ca"] = config.rootCA;
    }
    response["list-limit"] = config.listLimit;
    response["timeout"] = config.timeoutMillis;

    outputResponse(response);
    return DFXExitCode::SUCCESS;
}

ConfigListCommand::ConfigListCommand(CLI::App* config, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options))
{
    cmd = config->add_subcommand("list", "List available config contexts");
    cmd->alias("ls");
    cmd->callback([&]() { result = execute(); });
}

DFXExitCode ConfigListCommand::execute()
{
    std::string defaultContext;
    std::vector<std::string> contextNames;
    auto status = dfx::api::getAvailableContexts(options->configFilePath, defaultContext, contextNames);
    if (!status.OK()) {
        outputError(status);
        return DFXExitCode::FAILURE;
    }

    // If user is overloading on the command line, mark that context as the default
    CLI::App* app = cmd;
    while (app && app->get_parent()) {
        app = app->get_parent();
    }
    auto contextOpt = app->get_option_no_throw("--context");
    if (contextOpt != nullptr && contextOpt->count() > 0) {
        defaultContext = options->context;
    }

    json response = json::array();
    for (auto& name : contextNames) {
        json context = {{"context", name}};
        context["default"] = name.compare(defaultContext) == 0;
        response.push_back(context);
    }
    outputResponse(response);
    return DFXExitCode::SUCCESS;
}

ConfigSampleCommand::ConfigSampleCommand(CLI::App* config, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options)), advanced(false)
{
    cmd = config->add_subcommand("sample", "Write sample YAML config to stdout");
    cmd->add_flag("-a,--advanced", advanced, "Show an advanced sample format with multiple contexts");
    cmd->callback([&]() { result = execute(); });
}

DFXExitCode ConfigSampleCommand::execute()
{
    if (!advanced) {
        // Basic example format
        std::cout << "# DFX Sample YAML Configuration File (basic)\n"
                  << "# Save this default to: ~/.dfxcloud.yaml\n"
                  << "\n"
                  << "# This file contains only one configuration, to use multiple look at\n"
                  << "# the advanced configuration layout example.\n"
                  << "\n"
                  << "host: api.deepaffex.ai\n"
                  << "\n"
                  << "auth-email: your-email-id\n"
                  << "auth-password: your-password\n"
                  << "auth-org: your-org-identifier\n"
                  << "license: your-license\n"
                  << "study-id: study-id                 # (Optional) Used for operations requiring\n"
                  << "\n"
                  << "# If the below tokens are present, you do not require email, password, org, license\n"
                  << "#auth-token:   your-auth-token     # (Optional) Reusable from ./dfxcli login\n"
                  << "#device-token: your-device-token   # (Optional) Reusable from ./dfxcli register\n"
                  << std::endl;
    } else {
        std::cout << "# DFX Sample YAML Configuration File (advanced)\n"
                  << "# Save this default to: ~/.dfxcloud.yaml\n"
                  << "\n"
                  << "# This file can contain multiple contexts and this provides the default context\n"
                  << "# name to use when loading. Context can be overridden on command line with --context.\n"
                  << "context: rest   # default context name to use\n"
                  << "\n"
                  << "# Place defaults here which apply to all services or contexts and\n"
                  << "# explicitly override as needed for individual services or contexts\n"
                  << "verbose: 2\n"
                  << "timeout: 3000   # 3 seconds\n"
                  << "list-limit: 25  # Maximum number of records returned for list calls. Default: 25\n"
                  << "auth-email: your-email-id    # defining below is optional if here\n"
                  << "\n"
                  << "# Services define the end-point hosts and can be cloud instances or\n"
                  << "# standalone devices. They are used by contexts to provide host/port details.\n"
                  << "services:\n"
                  << "  - name: v2-rest\n"
                  << "    host: api.deepaffex.ai\n"
                  << "    transport-type: REST\n"
                  << "  - name: v2-websocket\n"
                  << "    host: api.deepaffex.ai\n"
                  << "    #skip-verify: true       # If TLS/SSL handshake can skip verification. Default: false\n"
                  << "    transport-type: WEBSOCKET\n"
                  << "  - name: v3-grpc\n"
                  << "    host: local-server.deepaffex.ai\n"
                  << "    port: 8443\n"
                  << "    transport-type: GRPC\n"
                  << "\n"
                  << "# Contexts provide the authentication details and link to the service hosts\n"
                  << "contexts:\n"
                  << "  - name: rest\n"
                  << "    service: v2-rest                   # links to service host/port name above\n"
                  << "    auth-email: your-email-id\n"
                  << "    auth-password: your-secret-password\n"
                  << "    auth-org: your-org-identifier\n"
                  << "    license: your-license\n"
                  << "    study-id: study-id                 # (Optional) Used for operations requiring\n"
                  << "\n"
                  << "    # Tokens can be cached in config to avoid login/register/unregister/logout on every request\n"
                  << "    # if provided, auth-email, auth-password, auth-org, license are optional\n"
                  << "    #auth-token:   your-auth-token     # (Optional) Reusable from ./dfxcli login\n"
                  << "    #device-token: your-device-token   # (Optional) Reusable from ./dfxcli register\n"
                  << "\n"
                  << "  - name: websocket\n"
                  << "    service: v2-websocket\n"
                  << "    auth-email: your-email-id\n"
                  << "    auth-password: your-secret-password\n"
                  << "    auth-org: your-org-identifier\n"
                  << "    license: your-license\n"
                  << "    study-id: study-id\n"
                  << "\n"
                  << "  - name: grpc\n"
                  << "    service: v3-grpc\n"
                  << "    auth-email: your-email-id\n"
                  << "    auth-password: your-secret-password\n"
                  << "    auth-org: your-org-identifier\n"
                  << "    license: your-license\n"
                  << "    study-id: study-id\n";
    }
    return DFXExitCode::SUCCESS;
}
