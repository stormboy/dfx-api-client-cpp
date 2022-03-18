// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/CloudAPI.hpp"
#include "dfx/api/utils/FileUtils.hpp"

#include "version_info.hpp"

#include <filesystem>
#include <fmt/format.h>
namespace fs = std::filesystem;

#ifdef WITH_CURL
#include "curl/curl.h"
#endif
#ifdef WITH_GRPC
#include "dfx/api/grpc/CloudGRPC.hpp"
#endif
#ifdef WITH_REST
#include "dfx/api/rest/CloudREST.hpp"
#endif
#ifdef WITH_WEBSOCKET
#include "dfx/api/websocket/CloudWebSocket.hpp"
#endif

// using namespace dfx::api;
using dfx::api::CloudAPI;
using dfx::api::CloudConfig;
using dfx::api::CloudStatus;
using dfx::api::DeviceAPI;
using dfx::api::LicenseAPI;
using dfx::api::MeasurementAPI;
using dfx::api::MeasurementStreamAPI;
using dfx::api::OrganizationAPI;
using dfx::api::ProfileAPI;
using dfx::api::SignalAPI;
using dfx::api::StudyAPI;
using dfx::api::UserAPI;

// NOLINTNEXTLINE(cert-err58-cpp)
const std::string CloudAPI::TRANSPORT_TYPE_REST = "REST";

// NOLINTNEXTLINE(cert-err58-cpp)
const std::string CloudAPI::TRANSPORT_TYPE_WEBSOCKET = "WEBSOCKET";

// NOLINTNEXTLINE(cert-err58-cpp)
const std::string CloudAPI::TRANSPORT_TYPE_GRPC = "GRPC";

uint16_t CloudAPI::numberCurlInstances = 0;
std::mutex CloudAPI::curlMutex;
std::string CloudAPI::clientIdentifier("CloudAPI");

#ifdef EMBED_CA_CERTS
extern "C" {
// This will be linked in if enabled but there is no header so just extern it here and
// leave it up the to the linker to resolve.
extern const char cacert[];
extern const size_t cacert_len;
};
#endif

CloudStatus CloudAPI::createInstance(CloudConfig& cloudConfig, std::shared_ptr<CloudAPI>& cloudAPI)
{
    CloudStatus status(CLOUD_TRANSPORT_FAILURE, "Transport unavailable");
    CloudConfig config = cloudConfig;

    config.rootCA = getRootCA(config);
    cloudConfig.rootCA = config.rootCA;
    std::string desiredTransportType = config.transportType;
    bool firstAvailableTransport = desiredTransportType.empty(); // If there was no desired transport, use first

#ifdef WITH_GRPC
    if (firstAvailableTransport || desiredTransportType == TRANSPORT_TYPE_GRPC) {
        auto instance = std::make_shared<dfx::api::grpc::CloudGRPC>(config);
        if (instance != nullptr) {
            status = instance->connect(config);
            if (status.OK()) {
                config.transportType = TRANSPORT_TYPE_GRPC;
                cloudAPI = instance;
            }
        }
    }
#endif

#ifdef WITH_REST
    if (firstAvailableTransport || desiredTransportType == TRANSPORT_TYPE_REST) {
        auto instance = std::make_shared<dfx::api::rest::CloudREST>(config);
        if (instance != nullptr) {
            status = instance->connect(config);
            if (status.OK()) {
                config.transportType = TRANSPORT_TYPE_REST;
                cloudAPI = instance;
            }
        }
    }
#endif

#ifdef WITH_WEBSOCKET
    if (firstAvailableTransport || desiredTransportType == TRANSPORT_TYPE_WEBSOCKET) {
        auto instance = std::make_shared<dfx::api::websocket::CloudWebSocket>(config);
        if (instance != nullptr) {
            status = instance->connect(config);
            if (status.OK()) {
                config.transportType = TRANSPORT_TYPE_WEBSOCKET;
                cloudAPI = instance;
            }
        }
    }
#endif

    if (cloudAPI == nullptr) {
        return status;
    }

    return CloudStatus(CLOUD_OK);
}

std::list<std::string> initializeTransportList()
{
    // Return transports in order of preference
    std::list<std::string> transports;
#ifdef WITH_GRPC
    transports.push_back(dfx::api::CloudAPI::TRANSPORT_TYPE_GRPC);
#endif
#ifdef WITH_REST
    transports.push_back(dfx::api::CloudAPI::TRANSPORT_TYPE_REST);
#endif
#ifdef WITH_WEBSOCKET
    transports.push_back(dfx::api::CloudAPI::TRANSPORT_TYPE_WEBSOCKET);
#endif
    return transports;
}

const std::list<std::string>& CloudAPI::getAvailableTransports()
{
    // initialization on first use - guarantees the list intact, and the list is holding
    // static references to strings so no temporaries which makes it nice for C-ABI.
    static auto transports = initializeTransportList();
    return transports;
}

#ifndef WITH_CURL
static CloudStatus fetchStandaloneRootCA(const CloudConfig& config, std::string& rootCA)
{
    return CloudStatus(CLOUD_OK);
}
#else

// DO NOT inline this function with a lambda, may lead to segfaults
extern "C" size_t curl_callback_handler(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static CloudStatus fetchStandaloneRootCA(const CloudConfig& config, std::string& rootCA)
{
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::string rootCAURL = fmt::format("http://{}/rootca", config.serverHost);

        curl_easy_setopt(curl, CURLOPT_URL, rootCAURL.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback_handler);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, config.timeoutMillis); // In seconds

        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            curl_easy_cleanup(curl);

            if (response_code == 200) {
                // Everything was good
                rootCA = readBuffer;
                return CloudStatus(CLOUD_OK);
            } else {
                return CloudStatus(
                    CLOUD_CURL_ERROR, "curl failed", res, fmt::format("Unexpected HTTP response: {}", response_code));
            }
        } else {
            curl_easy_cleanup(curl);
            return CloudStatus(CLOUD_CURL_ERROR, "curl failed", res, curl_easy_strerror(res));
        }
    }
    return CloudStatus(CLOUD_CURL_ERROR, "curl failed", CURLE_OUT_OF_MEMORY, curl_easy_strerror(CURLE_OUT_OF_MEMORY));
}

// Try our best to come up with the content of a Root CA file. If we are
// communicating with a standalone device, we will query the device and
// obtain the self-signed certificate. If we had a certificate embedded,
// we will use that. The caller can also provide their own in
// config.rootCA.
//
// In general, embedding a root ca is non-ideal but it does have the
// advantage of shipping a single file library - knowing that the cert
// will eventually need updating but the library consumer can always
// provide their own - or fall back to operating system provided.
std::string CloudAPI::getRootCA(const CloudConfig& config)
{
    std::string rootCA = config.rootCA;
    if (rootCA.empty()) {
#ifdef WITH_CURL
        // Check and see if we can fetch a rootca -- implying it was a standalone
        auto status = fetchStandaloneRootCA(config, rootCA);
#ifdef EMBED_CA_CERTS
        if (!status.OK()) {
            rootCA = std::string(cacert);
        }
#endif
#endif
    } else {
        // If the user provided a path, load the contents.
        if (rootCA.length() < 255 && fs::exists(rootCA)) {
            auto fileContent = dfx::api::utils::readFile(rootCA.c_str());
            rootCA = std::string(fileContent.begin(), fileContent.end());
        }
    }
    return rootCA;
}

const std::string& CloudAPI::getVersion()
{
    static std::string version(_id_versioninfo_dfxcloud());
    return version;
}

// Obtain the platform the library was compiled for.
const std::string& CloudAPI::getPlatform()
{
    static std::string deviceType(_id_versioninfo_dfxcloud_devicetype());
    return deviceType;
}

const std::string& CloudAPI::getClientIdentifier()
{
    return clientIdentifier;
}

CloudAPI::CloudAPI(const CloudConfig& config)
{
#ifdef WITH_CURL
    std::lock_guard<std::mutex> guard(CloudAPI::curlMutex);
    if (++CloudAPI::numberCurlInstances == 1) {
        // Needs to be called once for life of application - not thread safe
        //
        // LTO should handle eliminating these static global symbols so client can use
        // their own curl if they want
        curl_global_init(CURL_GLOBAL_ALL);
    }
#endif // WITH_CURL
}

CloudAPI::~CloudAPI()
{
#ifdef WITH_CURL
    std::lock_guard<std::mutex> guard(CloudAPI::curlMutex);
    if (++CloudAPI::numberCurlInstances == 0) {
        // Cleanup when the last reference goes out of scope - not thread safe
        curl_global_cleanup();
    }
#endif // WITH_CURL
}

std::shared_ptr<DeviceAPI> CloudAPI::device(const CloudConfig& config)
{
    return nullptr;
}

std::shared_ptr<LicenseAPI> CloudAPI::license(const CloudConfig& config)
{
    return nullptr;
}

std::shared_ptr<OrganizationAPI> CloudAPI::organization(const CloudConfig& config)
{
    return nullptr;
}

std::shared_ptr<MeasurementAPI> CloudAPI::measurement(const CloudConfig& config)
{
    return nullptr;
}

std::shared_ptr<MeasurementStreamAPI> CloudAPI::measurementStream(const CloudConfig& config)
{
    return nullptr;
}

std::shared_ptr<ProfileAPI> CloudAPI::profile(const CloudConfig& config)
{
    return nullptr;
}

std::shared_ptr<SignalAPI> CloudAPI::signal(const CloudConfig& config)
{
    return nullptr;
}

std::shared_ptr<StudyAPI> CloudAPI::study(const CloudConfig& config)
{
    return nullptr;
}

std::shared_ptr<UserAPI> CloudAPI::user(const CloudConfig& config)
{
    return nullptr;
}

#endif
