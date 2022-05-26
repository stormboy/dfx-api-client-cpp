// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/CloudConfig.hpp"

#include "dfx/api/CloudLog.hpp"
#include <cstdlib> // for getenv()
#include <ostream>

using namespace dfx::api;

#ifdef WITH_YAML
#include "yaml-cpp/yaml.h"

static CloudStatus loadCloudConfigYAMLFile(CloudConfig& config, const std::string& filePath);

static void loadCloudConfigYAMLContextNode(CloudConfig& config, std::string& service, YAML::Node& node);

static void loadCloudConfigYAMLServiceNode(CloudConfig& config, YAML::Node& node);
#endif

static inline bool ends_with(std::string const& value, std::string const& ending)
{
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

static std::string getEnv(const std::string& variable)
{
    std::string value;
#ifdef _WIN32
    // Avoid: warning C4996: 'getenv': This function or variable may be unsafe. Consider using _dupenv_s instead.
    char* buf = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buf, &sz, variable.c_str()) == 0 && buf != nullptr) {
        if (buf != nullptr) {
            value = std::string(buf);
        }
        free(buf);
    }
#else
    // getenv was not thread safe before C++11, we should be fine now.
    // NOLINTNEXTLINE(concurrency-mt-unsafe)
    value = std::getenv(variable.c_str());
#endif
    return value;
}

static std::string expandHomeTilde(const std::string& filePath)
{
    std::string path = filePath;

    // Expand ~ to $HOME if given
    if (filePath.length() > 0 && filePath.at(0) == '~') {
        auto envHome = getEnv("HOME");
        if (!envHome.empty()) {
            path = envHome + filePath.substr(1);
        } else {
            envHome = getEnv("USERPROFILE");
            if (!envHome.empty()) { // Windows
                path = envHome + filePath.substr(1);
            }
        }
    }

    return path;
}

CloudStatus dfx::api::loadCloudConfig(CloudConfig& config, const std::string& filePath)
{
    // Currently only support YAML, so if we don't have it we don't have any parser
#ifdef WITH_YAML
    std::string path = expandHomeTilde(filePath);
    if (ends_with(path, ".yaml")) {
        return loadCloudConfigYAMLFile(config, path);
    }
#endif
    return CloudStatus(CLOUD_CONFIG_UNSUPPORTED_CONTENT_TYPE, "Unable to load file: '" + filePath + "'");
}

CloudStatus dfx::api::getAvailableContexts(const std::string& filePath,
                                           std::string& defaultContext,
                                           std::vector<std::string>& contextNames)
{
#ifdef WITH_YAML
    std::string path = expandHomeTilde(filePath);
    if (ends_with(path, ".yaml")) {
        YAML::Node yaml = YAML::LoadFile(path);

        if (yaml["context"]) {
            defaultContext = yaml["context"].as<std::string>();
        }

        // Are there contexts defined?
        YAML::Node contextNodes = yaml["contexts"];
        if (contextNodes) {
            for (auto&& context : contextNodes) {
                auto contextName = context["name"].as<std::string>();
                contextNames.push_back(contextName);
            }
        }
        return CloudStatus(CLOUD_OK);
    }
#endif
    return CloudStatus(CLOUD_CONFIG_UNSUPPORTED_CONTENT_TYPE, "Unable to load file: '" + filePath + "'");
}

#ifdef WITH_YAML
static CloudStatus loadCloudConfigYAMLFile(CloudConfig& config, const std::string& filePath)
{
    try {
        YAML::Node yaml = YAML::LoadFile(filePath);

        config.timeoutMillis = 10 * 1000;
        config.secure = true;
        config.listLimit = 25;
        config.serverPort = 443;

        // What is the context of the configuration we are loading? If the user provided
        // us something in the CloudConfig that takes precedence over anything in the file.
        // If there was nothing provided though we look to load the context from
        // the file.
        std::string contextID = config.contextID;
        if (contextID.empty()) {
            if (yaml["context"]) {
                contextID = yaml["context"].as<std::string>();
            }
        }

        // Load the top level YAML node, which all configs inherit from both the context and service sets
        std::string serviceID;
        loadCloudConfigYAMLContextNode(config, serviceID, yaml);
        loadCloudConfigYAMLServiceNode(config, yaml);

        // If we have no context ID overlay the base configuration with we are done.
        if (contextID.empty()) {
            return CloudStatus(CLOUD_OK);
        } else {
            config.contextID = contextID;
        }

        // Are there contexts defined?
        YAML::Node contexts = yaml["contexts"];
        if (contexts) {
            for (auto&& context : contexts) {
                if (context["name"].as<std::string>() == contextID) {
                    loadCloudConfigYAMLContextNode(config, serviceID, context);
                    break; // We found the one we wanted and processed
                }
            }
        }

        if (serviceID.empty()) {
            return CloudStatus(CLOUD_OK);
        }

        // Are there services defined?
        YAML::Node services = yaml["services"];
        if (services) {
            for (auto&& service : services) {
                if (service["name"].as<std::string>() == serviceID) {
                    loadCloudConfigYAMLServiceNode(config, service);
                    break; // We found the one we wanted and processed
                }
            }
        }

        return CloudStatus(CLOUD_OK);

    } catch (YAML::BadFile err) {
        return CloudStatus(CLOUD_CONFIG_PARSE_ERROR, err.msg);
    } catch (YAML::ParserException ex) {
        return CloudStatus(CLOUD_CONFIG_PARSE_ERROR, ex.msg);
    }
}

static void loadCloudConfigYAMLContextNode(CloudConfig& config, std::string& service, YAML::Node& node)
{
    if (node["service"]) {
        service = node["service"].as<std::string>();
    }

    if (node["auth-email"]) {
        config.authEmail = node["auth-email"].as<std::string>();
    }
    if (node["auth-password"]) {
        config.authPassword = node["auth-password"].as<std::string>();
    }
    if (node["auth-org"]) {
        config.authOrg = node["auth-org"].as<std::string>();
    }
    if (node["study-id"]) {
        config.studyID = node["study-id"].as<std::string>();
    }
    if (node["license"]) {
        config.license = node["license"].as<std::string>();
    }
    if (node["device-token"]) {
        config.deviceToken = node["device-token"].as<std::string>();
    }
    if (node["auth-token"]) {
        config.authToken = node["auth-token"].as<std::string>();
    }
}

static void loadCloudConfigYAMLServiceNode(CloudConfig& config, YAML::Node& node)
{
    if (node["verbose"]) {
        // Originally, verbose was a boolean however it needed to be switch to a number to allow
        // for multiple levels of logging which is why this is attempted as a bool first which will
        // only success if it was true.
        auto verboseAsFlag = node["verbose"].as<bool>(false);
        if (verboseAsFlag) {
            // User asked for verbose as a true/false, but expected number so set WARN level
            // and warn them they need to change to a number.
            cloudLogSetLevel(CLOUD_LOG_LEVEL_WARNING);
            cloudLog(CLOUD_LOG_LEVEL_WARNING,
                     "config verbose expected integer value (not boolean), defaulted to WARNING=2");
        } else {
            auto verboseAsLevel = node["verbose"].as<uint8_t>(0);
            cloudLogSetLevel(verboseAsLevel);
            cloudLogSetEnabled(verboseAsLevel != 0);
        }
    }

    if (node["host"]) {
        config.serverHost = node["host"].as<std::string>();
    }
    if (node["port"]) {
        config.serverPort = node["port"].as<int>();
    }
    if (node["secure"]) {
        config.secure = node["secure"].as<bool>();
    }
    if (node["transportType"]) { // Legacy
        config.transportType = node["transportType"].as<std::string>();
    }
    if (node["transport-type"]) {
        config.transportType = node["transport-type"].as<std::string>();
    }
    if (node["skip-verify"]) {
        config.skipVerify = node["skip-verify"].as<bool>();
    }
    if (node["rootCA"]) { // Legacy
        config.rootCA = node["root-ca"].as<std::string>();
    }
    if (node["root-ca"]) {
        config.rootCA = node["root-ca"].as<std::string>();
    }
    if (node["timeout"]) {
        config.timeoutMillis = node["timeout"].as<int>();
    }
    if (node["list-limit"]) {
        config.listLimit = node["list-limit"].as<uint16_t>();
    }
}
#endif // WITH_YAML

std::ostream& operator<<(std::ostream& os, const CloudConfig& config)
{
    os << "host=" << config.serverHost << "\n";
    os << "secure=" << config.secure << "\n";
    if (config.serverPort != 0) {
        os << "port=" << config.serverPort << "\n";
    }
    if (!config.transportType.empty()) {
        os << "transport=" << config.transportType << "\n";
    }
    if (!config.authEmail.empty()) {
        os << "auth-email=" << config.authEmail << "\n";
    }
    if (!config.authPassword.empty()) {
        os << "auth-password=" << config.authPassword << "\n";
    }
    if (!config.authOrg.empty()) {
        os << "auth-org=" << config.authOrg << "\n";
    }
    if (!config.license.empty()) {
        os << "license=" << config.license << "\n";
    }
    if (!config.studyID.empty()) {
        os << "study-id=" << config.studyID << "\n";
    }
    int rootCALength = config.rootCA.length();
    if (rootCALength > 0) {
        int max = rootCALength;
        if (max > 80) {
            max = 80;
        }
        os << "root-ca=" << config.rootCA.substr(0, max);
        if (max != rootCALength) {
            os << " ...truncated";
        }
        os << "\n";
    }
    if (config.timeoutMillis != 0) {
        os << "timeout=" << config.timeoutMillis << "\n";
    }
    return os;
}
