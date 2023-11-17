// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/rest/CloudREST.hpp"
#include "dfx/api/CloudLog.hpp"

#include "dfx/api/rest/DeviceREST.hpp"
#include "dfx/api/rest/LicenseREST.hpp"
#include "dfx/api/rest/MeasurementREST.hpp"
#include "dfx/api/rest/MeasurementStreamREST.hpp"
#include "dfx/api/rest/OrganizationREST.hpp"
#include "dfx/api/rest/ProfileREST.hpp"
#include "dfx/api/rest/SignalREST.hpp"
#include "dfx/api/rest/StudyREST.hpp"
#include "dfx/api/rest/UserREST.hpp"

#include "dfx/api/validator/CloudValidator.hpp"

#include "dfx/api/utils/HexDump.hpp"

#include "curl/curl.h"
#include "fmt/args.h" // for fmt::dynamic_format_arg_store
#include "nlohmann/json.hpp"
#include "openssl/err.h"
#include "openssl/ssl.h"

#include <memory>
#include <sstream>
#include <string>

using namespace dfx::api;
using namespace dfx::api::rest;

CloudREST::CloudREST(const CloudConfig& config) : CloudAPI(config) {}

std::string CloudREST::getAuthToken(const CloudConfig& config) {
    if ( !config.authToken.empty() ) {
        return config.authToken;
    } else {
        return config.deviceToken;
    }
}

CloudStatus CloudREST::connect(const CloudConfig& config)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, connect(config));

    // Nothing required since REST will make a connection per-call. It could be
    // enhanced to attempt a call to the server and return an invalid status.
    return CloudStatus(CLOUD_OK);
}

CloudStatus CloudREST::login(CloudConfig& config)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, login(config));

    nlohmann::json request = {
        {"Email", config.authEmail}, {"Password", config.authPassword}, {"Identifier", config.authOrg}};
    nlohmann::json response;

    if (!config.authMFAToken.empty()) {
        request["MFAToken"] = config.authMFAToken;
    }

    // https://dfxapiversion10.docs.apiary.io/#reference/0/users/login
    auto result = CloudREST::performRESTCall(config, web::Users::Login, config.deviceToken, {}, request, response);
    if (result.OK()) {
        config.authToken = response["Token"];

        if (response.contains("RefreshToken")) {
            config.userRefreshToken = response["RefreshToken"];
        }
    }
    return result;
}

CloudStatus CloudREST::loginWithToken(CloudConfig& config, std::string& token)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, loginWithToken(config, token));

    nlohmann::json request = {{"Token", token}};
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/organizations/login-with-token
    auto result = CloudREST::performRESTCall(
        config, web::Organizations::LoginWithToken, config.deviceToken, {}, request, response);
    if (result.OK()) {
        token = response["Token"];
    }

    return result;
}

// REST does not have a logout for users - so calling can't fail
CloudStatus CloudREST::logout(CloudConfig& config)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, logout(config));
    return CloudStatus(CLOUD_OK);
}

CloudStatus CloudREST::registerDevice(CloudConfig& config,
                                      const std::string& appName,
                                      const std::string& appVersion,
                                      const uint16_t tokenExpiresInSeconds,
                                      const std::string& tokenSubject)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, registerDevice(config, appName, appVersion));
    auto deviceType = CloudAPI::getPlatform();
    deviceType = "LINUX";

    nlohmann::json request = {{"Key", config.license},
                              {"DeviceTypeID", deviceType},
                              {"Name", appName},
                              {"Identifier", CloudAPI::getClientIdentifier()},
                              {"Version", appVersion}};
    nlohmann::json response;

    if (tokenExpiresInSeconds > 0) {
        request["TokenExpiresIn"] = tokenExpiresInSeconds;
    }
    if (!tokenSubject.empty()) {
        request["TokenSubject"] = tokenSubject;
    }

    // https://dfxapiversion10.docs.apiary.io/#reference/0/organizations/register-license
    auto result = CloudREST::performRESTCall(config, web::Organizations::RegisterLicense, "", {}, request, response);

    if (result.OK()) {
        config.deviceID = response["DeviceID"];
        config.deviceToken = response["Token"];
        if (response.contains("RefreshToken")) {
            config.deviceRefreshToken = response["RefreshToken"];
        }
        // auto roleID = response["RoleID"];
        // auto userID = response["UserID"];
    }
    return result;
}

CloudStatus CloudREST::unregisterDevice(CloudConfig& config)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, unregisterDevice(config));

    nlohmann::json request;
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/organizations/unregister-license
    auto result = CloudREST::performRESTCall(config, web::Organizations::UnregisterLicense, "", {}, request, response);
    return result;
}

CloudStatus CloudREST::verifyToken(const CloudConfig& config, std::string& response)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, verifyToken(config, response));

    nlohmann::json request;
    nlohmann::json jsonResponse;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/general/verify-token
    auto result =
        CloudREST::performRESTCall(config, web::General::VerifyToken, getAuthToken(config), {}, request, jsonResponse);
    if (result.OK()) {
        response = jsonResponse.dump();
    }

    return result;
}

CloudStatus CloudREST::renewToken(const CloudConfig& config, std::string& token, std::string& refreshToken)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, renew(config, token, refreshToken));

    nlohmann::json request = {{"Token", token}, {"RefreshToken", refreshToken}};
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/auths/renew
    auto result = CloudREST::performRESTCall(config, web::Auths::RenewToken, config.authToken, {}, request, response);
    if (result.OK()) {
        token = response["Token"];
        refreshToken = response["RefreshToken"];
    }

    return result;
}

CloudStatus CloudREST::switchEffectiveOrganization(CloudConfig& config, const std::string& organizationID)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE);
}

static size_t curlCallbackFunction(void* contents, size_t size, size_t nmemb, void* userp)
{
    (static_cast<std::string*>(userp))->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

CloudStatus CloudREST::performRESTCall(const CloudConfig& config,
                                       const dfx::api::web::WebServiceDetail& details,
                                       const std::string& authToken,
                                       const std::vector<std::string>& urlArgs,
                                       const nlohmann::json& payload,
                                       nlohmann::json& response)
{
    return performRESTCall(config, details, authToken, urlArgs, "", payload, response);
}

#ifndef NDEBUG
// callback for curl CURLOPT_DEBUGFUNCTION with tracing enabled
static int custom_curl_trace_callback(CURL* handle, curl_infotype type, char* data, size_t size, void* userp)
{
    const char* text;
    (void)handle; /* prevent compiler warning */
    (void)userp;

    switch (type) {
        case CURLINFO_TEXT:
            cloudLog(CLOUD_LOG_LEVEL_DEBUG, "REST: %s", data);
        default: /* in case a new one is introduced to shock us */
            return 0;

        case CURLINFO_HEADER_OUT:
            text = "REST => Send header\n";
            break;
        case CURLINFO_DATA_OUT:
            text = "REST => Send data\n";
            break;
        case CURLINFO_SSL_DATA_OUT:
            text = "REST => Send SSL data\n";
            break;
        case CURLINFO_HEADER_IN:
            text = "REST <= Recv header\n";
            break;
        case CURLINFO_DATA_IN:
            text = "REST <= Recv data\n";
            break;
        case CURLINFO_SSL_DATA_IN:
            text = "REST <= Recv SSL data\n";
            break;
    }

    if (cloudLogIsActive(CLOUD_LOG_LEVEL_TRACE)) {
        std::string hexDump = dfx::api::utils::hexDump(text, (void*)data, size);
        cloudLog(CLOUD_LOG_LEVEL_TRACE, hexDump.c_str());
    } else {
        cloudLog(CLOUD_LOG_LEVEL_DEBUG, text);
    }

    return 0;
}
#endif

CloudStatus CloudREST::performRESTCall(const CloudConfig& config,
                                       const dfx::api::web::WebServiceDetail& details,
                                       const std::string& authToken,
                                       const std::vector<std::string>& urlArgs,
                                       const std::string& query,
                                       const nlohmann::json& payload,
                                       nlohmann::json& response)
{
    if (details.urlArgCount != urlArgs.size()) {
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "Expected urlArg count did not match");
    }

    fmt::dynamic_format_arg_store<fmt::format_context> store;
    for (const auto& arg : urlArgs) {
        store.push_back(arg);
    }

    auto urlPath = fmt::vformat(details.urlPath, store);

    std::string url;
    // NOLINTNEXTLINE(bugprone-branch-clone)      one is https, one is not... guess it could be restructured
    if (config.secure) {
        url = fmt::format("https://{}:{}/{}", config.serverHost, config.serverPort, urlPath);
    } else {
        url = fmt::format("http://{}:{}/{}", config.serverHost, config.serverPort, urlPath);
    }

    if (!query.empty()) {
        url = fmt::format("{}?{}", url, query);
    }

    cloudLog(CLOUD_LOG_LEVEL_INFO, "REST Request: %s\n", url.c_str());

    CURL* curl;
    CURLcode res(CURLE_OUT_OF_MEMORY);
    std::string readBuffer;

    long httpResponseCode = 0;
    curl = curl_easy_init();
    if (curl) {
        auto curlHeaders = curl_slist_append(nullptr, "Content-Type: application/json");

        std::string payloadString("");
        if (!payload.is_null()) {
            payloadString = payload.dump();
        }

        if (curlHeaders != nullptr) {
            // curl_slist returns the first item or nullptr... we won't leak
            // memory, but it is sort of best effort add after the first an
            // expect server to complain on missing headers it wanted.
            std::string contentLength = "Content-Length: " + std::to_string(payloadString.length());
            curl_slist_append(curlHeaders, contentLength.c_str());

            if (authToken.size() > 0) {
                std::string authorization = "Authorization: Bearer " + authToken;
                curl_slist_append(curlHeaders, authorization.c_str());
            }

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallbackFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, config.timeoutMillis);

            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaders);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // enable following HTTP 3xx redirects

            // Should we validate the TLS connection?
            // Informative read on verification https://curl.se/docs/sslcerts.html
            if (!config.skipVerify) {
                // TLS verify the certificate is authentic by verifying the chain of certificates
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, !config.skipVerify);

                // TLS verify that the server cert is for the server it is known as
                //    * WARNING: disabling hostname validation also disables SNI
                // which sporadically causes the server to terminate the connection.
                // https://github.com/curl/curl/issues/6347#issuecomment-748526449
                //
                // Currently, on Darwin conan forces use of the openssl package when
                // building libcurl to work around.
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, !config.skipVerify);

                if (!config.rootCA.empty()) {
                    bool rootCASpecifiesFile = false;
                    if (rootCASpecifiesFile) {
                        curl_easy_setopt(curl, CURLOPT_CAINFO, config.rootCA.c_str());
                    } else {
                        // To handle in-memory certificates there are actually two possible ways.
                        // The original method shown in https://curl.se/libcurl/c/cacertinmem.html
                        // which uses a callback and is more difficult to use as there is no easy
                        // way to transfer the config.rootCA into the callback. The newer method,
                        // https://curl.se/libcurl/c/CURLOPT_CAINFO_BLOB.html is inline and nicer
                        // for state.
                        struct curl_blob blob;
                        blob.data = const_cast<char*>(config.rootCA.c_str()); // life valid until return
                        blob.len = config.rootCA.length();
                        blob.flags = CURL_BLOB_COPY;
                        curl_easy_setopt(curl, CURLOPT_CAINFO_BLOB, &blob); // curl_blob must be PEM format
                    }
                }
            }

#ifndef NDEBUG
            if (cloudLogIsActive(CLOUD_LOG_LEVEL_DEBUG)) {
                curl_easy_setopt(curl, CURLOPT_VERBOSE, true);
                curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, custom_curl_trace_callback);
            }
#endif

            std::string agentID("dfxcloud/" + getVersion());
            curl_easy_setopt(curl, CURLOPT_USERAGENT, agentID.c_str());

            if (!payloadString.empty()) {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payloadString.length());
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadString.c_str());
            }
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, details.httpOption.c_str()); // GET, DELETE, POST, etc.

            // Perform the call to the server
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                // Request failed - which could occur for something like a CURLE_SSL_CONNECT_ERROR
                auto resResponse = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpResponseCode);

                cloudLog(CLOUD_LOG_LEVEL_WARNING,
                         "REST Failed: res=%d, httpResponseCode=%d, respCode=%d, msg=%s\n",
                         res,
                         httpResponseCode,
                         resResponse,
                         curl_easy_strerror(res));

                return CloudStatus(CLOUD_INTERNAL_ERROR, "Request failed", httpResponseCode, curl_easy_strerror(res));
            } else {
                // Request was successful, capture the http server response
                auto resResponse = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpResponseCode);
            }

            curl_slist_free_all(curlHeaders);
        }

        curl_easy_cleanup(curl);
    }

    // If there is a CODE && Message should peel and inject
    if (res == CURLE_OK) {
        response = nlohmann::json::parse(readBuffer, nullptr, false);

        if (cloudLogIsActive(CLOUD_LOG_LEVEL_INFO)) {
            auto responseDump = response.dump();
            cloudLog(CLOUD_LOG_LEVEL_INFO, "REST Response: %s\n", responseDump.c_str());
        }

        if (response.is_discarded()) {
            return CloudStatus(CLOUD_CURL_ERROR, "curl json failed", res, curl_easy_strerror(res));
        } else {
            // Are we getting back an error?
            if (response.contains("Code") && response.contains("Message")) {
                std::string responseCode = response["Code"];
                std::string responseMessage = response["Message"];
                if (responseCode.compare("BAD_REQUEST") == 0) {
                    return CloudStatus(CLOUD_BAD_REQUEST, responseMessage);
                } else if (responseCode.compare("USER_ALREADY_EXISTS") == 0) {
                    return CloudStatus(CLOUD_RECORD_ALREADY_EXISTS, responseMessage);
                } else if (responseCode.compare("VALIDATION_ERROR") == 0) {
                    // Have something that looks like:
                    //  {"Code":"VALIDATION_ERROR","Errors":{"Status":[["VALID_PROPERTIES",["ACTIVE","INACTIVE"]]]},"Message":""}
                    auto errors = response["Errors"];
                    std::stringstream ss;
                    for (const auto& error : errors.items()) {
                        ss << "\"" << error.key() << "\"";
                        for (auto propertyError : error.value()) {
                            if (propertyError.size() == 2 && propertyError[0] == "VALID_PROPERTIES") {
                                ss << " field valid properties include: ";
                                auto validValues = propertyError[1];
                                for (auto index = 0; index < validValues.size(); index++) {
                                    if (index != 0) {
                                        ss << ", ";
                                    }
                                    ss << validValues[index];
                                }
                                responseMessage = ss.str();
                            } else {
                                // Not sure what the format of this is to make it look pretty, give user something
                                responseMessage = errors.dump();
                            }
                        }
                    }
                    return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, responseMessage);
                } else if (responseCode.compare("NOT_FOUND") == 0) {
                    return CloudStatus(CLOUD_RECORD_NOT_FOUND, responseMessage);
                } else {
                    cloudLog(CLOUD_LOG_LEVEL_INFO,
                             "REST Failed: http response=%d, reason=%s\n",
                             httpResponseCode,
                             responseCode.c_str());
                }
            }
        }

        switch (httpResponseCode) {
            case 200:
                return CloudStatus(CLOUD_OK);
            case 500:
                return CloudStatus(CLOUD_INTERNAL_ERROR, // 500: INTERNAL_ERROR
                                   "Internal server error",
                                   httpResponseCode,
                                   "");
            case 403:
                return CloudStatus(CLOUD_USER_NOT_AUTHORIZED, // 400: RESTRICTED
                                   "User does not have permission for request",
                                   httpResponseCode,
                                   "");
        }
        return CloudStatus(CLOUD_INTERNAL_ERROR,
                           std::to_string(httpResponseCode)); // It was not successful, not sure why it failed though
    } else {
        return CloudStatus(CLOUD_CURL_ERROR, "curl failed", res, curl_easy_strerror(res));
    }
}

std::string CloudREST::buildListFilterQuery(const std::map<std::string, std::string>* filterCriteria,
                                            uint16_t offset,
                                            uint16_t limit)
{
    std::stringstream ss;
    ss << "Offset=" << offset << "&Limit=" << limit;
    if (filterCriteria != nullptr) {
        for (const auto& criteria : *filterCriteria) {
            if (criteria.first != "Limit" && criteria.first != "Offset") {
                ss << "&" << criteria.first << "=" << criteria.second;
            }
        }
    }

    return ss.str();
}

std::shared_ptr<DeviceAPI> CloudREST::device(const CloudConfig& config)
{
    return std::make_shared<DeviceREST>();
}

std::shared_ptr<LicenseAPI> CloudREST::license(const CloudConfig& config)
{
    return std::make_shared<LicenseREST>();
}

std::shared_ptr<MeasurementAPI> CloudREST::measurement(const CloudConfig& config)
{
    return std::make_shared<MeasurementREST>();
}

std::shared_ptr<MeasurementStreamAPI> CloudREST::measurementStream(const CloudConfig& config)
{
    return std::make_shared<MeasurementStreamREST>(config);
}

std::shared_ptr<OrganizationAPI> CloudREST::organization(const CloudConfig& config)
{
    return std::make_shared<OrganizationREST>();
}

std::shared_ptr<ProfileAPI> CloudREST::profile(const CloudConfig& config)
{
    return std::make_shared<ProfileREST>();
}

std::shared_ptr<SignalAPI> CloudREST::signal(const CloudConfig& config)
{
    return std::make_shared<SignalREST>();
}

std::shared_ptr<StudyAPI> CloudREST::study(const CloudConfig& config)
{
    return std::make_shared<StudyREST>();
}

std::shared_ptr<UserAPI> CloudREST::user(const CloudConfig& config)
{
    return std::make_shared<UserREST>(config, std::static_pointer_cast<CloudREST>(shared_from_this()));
}

const std::string& CloudREST::getTransportType()
{
    return CloudAPI::TRANSPORT_TYPE_REST;
}

//     *	status			| Description
//     *	---------------	|-----------------------------
//     *	ACTIVE			| Online and fully operational
//     *	MAINTENANCE		| Offline for maintenance
//     *	ERROR			| Offline due to an error
//     *	LATENCY			| API is experience latency or a general slowdown
CloudStatus CloudREST::getServerStatus(CloudConfig& config, std::string& response)
{
    // The REST implementation has an end-point available for status that we can hit.
    nlohmann::json request;
    nlohmann::json jsonResponse;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/general/status
    auto result =
        CloudREST::performRESTCall(config, web::General::Status, config.deviceToken, {}, request, jsonResponse);
    if (result.OK()) {
        response = jsonResponse.dump();
    }
    return result;
}