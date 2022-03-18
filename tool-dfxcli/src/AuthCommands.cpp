// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/cli/AuthCommands.hpp"
#include "nlohmann/json.hpp"

using dfx::api::CloudStatus;
using dfx::api::License;
using nlohmann::json;

AuthLoginCommand::AuthLoginCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options)), registerDevice(false)
{
    cmd = get->add_subcommand("login", "Login to perform operations")->group("Authentication Commands");
    cmd->add_flag("-r,--register", registerDevice, "Include device registration in auth-token")->capture_default_str();
    cmd->callback([&]() {
        if (registerDevice) { // Full pipeline
            result = cleanup(setupAndExecute());
        } else { // Skip registration of device
            result = cleanup(DFXAppCommand::execute(login(connect(loadConfig()))));
        }
    });
}

DFXExitCode AuthLoginCommand::execute()
{
    outputResponse({{"auth-token", config.authToken}});
    config.authToken = ""; // Clear it to avoid releasing it back to the server
    return DFXExitCode::SUCCESS;
}

AuthLogoutCommand::AuthLogoutCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options))
{
    cmd = get->add_subcommand("logout", "Logout to invalidate auth session tokens")->group("Authentication Commands");
    cmd->add_option("tokens", authTokens, "Auth tokens to logout")->required(true);
    cmd->callback([&]() { result = cleanup(DFXAppCommand::execute(login(connect(loadConfig())))); });
}

DFXExitCode AuthLogoutCommand::execute()
{
    auto originalAuthToken = config.authToken;
    for (auto& token : authTokens) {
        config.authToken = token;
        auto status = client->logout(config);
        if (!status.OK()) {
            outputError(status);
            return DFXExitCode::FAILURE;
        }
    }
    config.authToken = originalAuthToken; // Restore for cleanup
    outputResponse(json::object());
    return DFXExitCode::SUCCESS;
}

AuthDeviceRegisterCommand::AuthDeviceRegisterCommand(CLI::App* get,
                                                     std::shared_ptr<Options> options,
                                                     DFXExitCode& result)
    : DFXAppCommand(std::move(options))
{
    cmd =
        get->add_subcommand("register", "Register device (required for measurement)")->group("Authentication Commands");

    cmd->callback([&]() {
        // Skip the login
        result = cleanup(DFXAppCommand::execute(registerDevice(connect(loadConfig()))));
    });
}

DFXExitCode AuthDeviceRegisterCommand::execute()
{
    outputResponse({{"device-token", config.deviceToken}});
    config.deviceToken = ""; // Clear it to avoid releasing it back to the server
    return DFXExitCode::SUCCESS;
}

AuthDeviceUnregisterCommand::AuthDeviceUnregisterCommand(CLI::App* get,
                                                         std::shared_ptr<Options> options,
                                                         DFXExitCode& result)
    : DFXAppCommand(std::move(options))
{
    cmd = get->add_subcommand("unregister", "Unregister device tokens (release device license)")
              ->group("Authentication Commands");
    cmd->add_option("tokens", deviceTokens, "Device tokens to unregister")->required(true);
    cmd->callback([&]() { result = cleanup(DFXAppCommand::execute(login(connect(loadConfig())))); });
}

DFXExitCode AuthDeviceUnregisterCommand::execute()
{
    auto originalDeviceToken = config.deviceToken;
    for (auto& token : deviceTokens) {
        config.deviceToken = token;
        auto status = client->unregisterDevice(config);
        if (!status.OK()) {
            outputError(status);
            return DFXExitCode::FAILURE;
        }
    }
    config.deviceToken = originalDeviceToken; // Restore for cleanup
    outputResponse(json::object());
    return DFXExitCode::SUCCESS;
}