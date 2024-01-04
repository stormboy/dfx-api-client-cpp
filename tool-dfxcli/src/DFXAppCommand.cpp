// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/cli/DFXAppCommand.hpp"
#include "dfx/api/CloudAPI.hpp"
#include "dfx/api/CloudLog.hpp"
#include <fmt/format.h>
#include <nlohmann/json.hpp>

using nlohmann::json;

// Releasing tokens is handled via callback registration to ensure proper release
// if the command is interrupted. If the program had implicitly logged in and
// registered, we want to release those tokens before terminating.
std::vector<std::function<void()>> callbacks;
void addCleanupOperation(std::function<void()> callback)
{
    callbacks.push_back(callback);
}
void runCleanupOperations()
{
    for (auto& func : callbacks) {
        func();
    }
    callbacks.clear(); // Only need to run once
}

DFXAppCommand::DFXAppCommand(std::shared_ptr<Options> options) : options(std::move(options)), offset(0), limit(25) {}

void DFXAppCommand::outputResponse(const nlohmann::json& json)
{
    outputFormatted(std::cout, json);
}

void DFXAppCommand::outputError(const nlohmann::json& json)
{
    // It is an error, but it will be the only valid "json" output which we really want on
    // std::out and not std::cerr. Other messages which are not json formatted will potentially
    // show up on std::cerr.
    outputFormatted(std::cout, json);
}

void DFXAppCommand::outputFormatted(std::ostream& os, const nlohmann::json& json)
{
    if (options->table) {
        for (const auto& item : json.items()) {
            os << item.key() << "=" << item.value() << std::endl;
        }
    } else {
        if (options->pretty) {
            os << json.dump(4) << std::endl;
        } else {
            os << json.dump() << std::endl;
        }
    }
}

DFXExitCode DFXAppCommand::loadConfig()
{
    // The options are specified at the root layer, this cmd is likely one or
    // two layers down so walk back up to the root (who's parent is nullptr).
    CLI::App* app = cmd;
    while (app && app->get_parent()) {
        app = app->get_parent();
    }

    // The context is used prior to the
    auto contextOpt = app->get_option_no_throw("--context");
    if (contextOpt != nullptr && contextOpt->count() > 0) {
        config.contextID = options->context;

        // If user has asked us to use a context, scan our available contexts to ensure that's even feasible
        std::string defaultContext;
        std::vector<std::string> contextNames;
        auto status = dfx::api::getAvailableContexts(options->configFilePath, defaultContext, contextNames);
        if (!status.OK()) {
            outputError(status);
            return DFXExitCode::FAILURE;
        }

        if (std::find(contextNames.begin(), contextNames.end(), options->context) == contextNames.end()) {
            dfx::api::CloudStatus status(CLOUD_PARAMETER_VALIDATION_ERROR,
                                         "Requested context '" + options->context + "' not found");
            outputError(status);
            return DFXExitCode::FAILURE;
        }
    }

    auto status = dfx::api::loadCloudConfig(config, options->configFilePath);
    if (status.code != CLOUD_OK) {
        outputError(status);
        return DFXExitCode::FAILURE;
    }

    // We have succesfully loaded the configuration - now apply all of our overloads
    auto verboseOpt = app->get_option_no_throw("--verbose");
    if (verboseOpt != nullptr && verboseOpt->count() > 0) {
        dfx::api::cloudLogSetEnabled(true);
        dfx::api::cloudLogSetLevel(options->verbose);
    }

    auto timeoutOpt = app->get_option_no_throw("--timeout");
    if (timeoutOpt != nullptr && timeoutOpt->count() > 0) {
        config.timeoutMillis = options->timeout;
    }

    auto hostOpt = app->get_option_no_throw("--host");
    if (hostOpt != nullptr && hostOpt->count() > 0) {
        config.serverHost = options->host;
    }

    auto portOpt = app->get_option_no_throw("--port");
    if (portOpt != nullptr && portOpt->count() > 0) {
        config.serverPort = options->port;
    }

    return DFXExitCode::SUCCESS;
}

DFXExitCode DFXAppCommand::connect(DFXExitCode code)
{
    if (code != DFXExitCode::SUCCESS) {
        return code;
    }

    auto status = dfx::api::CloudAPI::createInstance(config, client);
    if (status.code != CLOUD_OK) {
        outputError(status);
        return DFXExitCode::FAILURE;
    }

    return DFXExitCode::SUCCESS;
}

DFXExitCode DFXAppCommand::registerDevice(DFXExitCode code)
{
    if (code != DFXExitCode::SUCCESS) {
        return code;
    }

    const std::string appName("dfxcli");
    const std::string appVersion("2.0");

    // Most end-points require a device token, but some don't need so skip if there is no license provided
    if (config.deviceToken.empty() && !config.license.empty()) {
        uint16_t tokenExpiresInSeconds = 3600;
        std::string tokenSubject = "";
        auto status = client->registerDevice(config, appName, appVersion, tokenExpiresInSeconds, tokenSubject);
        if (status.code != CLOUD_OK) {
            outputError(status);
            return DFXExitCode::FAILURE;
        }

        addCleanupOperation([&]() {
            if (config.transportType.compare("REST") != 0) { // REST was failing with RESTRICTED_ACCESS
                auto status = client->unregisterDevice(config);
                if (!status.OK()) {
                    // Unable to unregisterDevice... we tried, not much we can do
                    if (options->verbose) {
                        std::cerr << "WARN: Unable to unregister device. Error=" << status.message << std::endl;
                    }
                }
            }
        });
    }
    return DFXExitCode::SUCCESS;
}

DFXExitCode DFXAppCommand::login(DFXExitCode code)
{
    if (code != DFXExitCode::SUCCESS) {
        return code;
    }

    if (config.authToken.empty()) {
        auto status = client->login(config);
        if (status.code != CLOUD_OK) {
            outputError(status);
            return DFXExitCode::FAILURE;
        }

        addCleanupOperation([&]() {
            if (!config.authToken.empty()) {
                auto status = client->logout(config);
                if (!status.OK()) {
                    // Unable to logout... we tried, not much we can do
                    if (options->verbose) {
                        std::cerr << "WARN: Unable to logout. Error=" << status.message << std::endl;
                    }
                }
                config.authToken = "";
            }
        });
    }

    return DFXExitCode::SUCCESS;
}

DFXExitCode DFXAppCommand::execute(DFXExitCode code)
{
    if (code != DFXExitCode::SUCCESS) {
        return code;
    }
    return execute();
}

DFXExitCode DFXAppCommand::setupAndExecute()
{
    return execute(login(registerDevice(connect(loadConfig()))));
}

DFXExitCode DFXAppCommand::cleanup(DFXExitCode code)
{
    runCleanupOperations();
    client = nullptr;
    return code;
}

CLI::App* DFXAppCommand::createGetCommand(CLI::App* parent, const std::string& name, bool includeIDs)
{
    std::string upperCaseName(name);
    if (!upperCaseName.empty()) {
        upperCaseName[0] = toupper(upperCaseName[0]);
    }

    auto cmd = parent->add_subcommand(name, fmt::format("Display {} information", name));

    cmd->add_flag("-f,--full", full, "Return full record details");
    cmd->add_option("-o,--offset", offset, fmt::format("Offset to start retrieving {}s", cmd->get_name()))
        ->capture_default_str();
    cmd->add_option("-l,--limit", limit, fmt::format("Limit on number of {}s to retrieve", cmd->get_name()))
        ->capture_default_str();
    if (includeIDs) {
        auto optionIDs = cmd->add_option("ids", ids, fmt::format("{} IDs to retrieve", upperCaseName));
        optionIDs->excludes("--offset");
        optionIDs->excludes("--limit");
    }

    return cmd;
}