// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/websocket/json/CloudWebSocketJson.hpp"
#include "dfx/api/CloudLog.hpp"

#include "dfx/api/utils/HexDump.hpp"
#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/websocket/json/DeviceWebSocketJson.hpp"
#include "dfx/api/websocket/json/LicenseWebSocketJson.hpp"
#include "dfx/api/websocket/json/MeasurementStreamWebSocketJson.hpp"
#include "dfx/api/websocket/json/MeasurementWebSocketJson.hpp"
#include "dfx/api/websocket/json/OrganizationWebSocketJson.hpp"
#include "dfx/api/websocket/json/ProfileWebSocketJson.hpp"
#include "dfx/api/websocket/json/StudyWebSocketJson.hpp"
#include "dfx/api/websocket/json/UserWebSocketJson.hpp"

#include "dfx/api/web/WebServiceDetail.hpp"

#include "dfx/websocket/WebSocket.hpp"

#include "fmt/format.h"
#include "nlohmann/json.hpp"

#include <string>
#include <vector>

#ifdef WITH_CURL
#include "curl/curl.h"
#endif

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <algorithm>
#include <chrono>
#include <thread>
using namespace dfx::api;
using namespace dfx::api::websocket::json;
using namespace dfx::websocket;

extern "C" {
void CloudWebSocketJsonCallback(const WebSocketEvent& event, void* userData)
{
    auto self = static_cast<CloudWebSocketJson*>(userData);
    self->handleEvent(event);
}
}

CloudWebSocketJson::CloudWebSocketJson(const CloudConfig& config)
    : CloudAPI(config), lastTransactionID(1), closedReason(""), closed(true)
{
}

CloudStatus CloudWebSocketJson::connect(const CloudConfig& config)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, connect(config));

    closed = false;

    if (cloudLogEnabled()) {
        auto logLevel = cloudLogLevel();
        webSocket = WebSocket::create(logLevel, [](uint8_t level, const char* message) { cloudLog(level, message); });
    } else {
        // No logging needed
        webSocket = WebSocket::create(0, nullptr);
    }

    webSocket->setRootCertificate(getRootCA(config));
    webSocket->setEventCallback(&CloudWebSocketJsonCallback, this);

    std::string wssURL = fmt::format("wss://{}:{}", config.serverHost, config.serverPort);
    webSocket->open(wssURL, "json");

    // Handle the special case that the WebSocket dies during the open, the handler thread won't have
    // started yet and no threads will be waiting on the open yet.
    if (webSocket->getState() == WebSocketState::CLOSED) {
        closedReason = "Failed to open WebSocket";
        closed = true;
        return CloudStatus(CLOUD_INTERNAL_ERROR, closedReason);
    }

    // The call to open will kick of a bunch of asynchronous activity that will eventually trigger
    // a call to handleEvent(OPEN) hopefully, or error but in any case we wait.
    std::unique_lock<std::mutex> lock(mutexOpen);
    if (!closed) {
        cvWebSocketOpen.wait(lock);
        return CloudStatus(CLOUD_OK);
    } else {
        return CloudStatus(CLOUD_INTERNAL_ERROR, closedReason);
    }
}

CloudWebSocketJson::~CloudWebSocketJson()
{
    // The webSocket thread may notify us via a callback so we need to ensure the
    // webSocket is properly closed and its associated thread terminates prior to
    // allowing the memory associated with this instance from being released.
    if (webSocket != nullptr) {
        webSocket->close();
    }
}

void CloudWebSocketJson::handleEvent(const WebSocketEvent& event)
{
#ifndef NDEBUG
    if (cloudLogIsActive(CLOUD_LOG_LEVEL_TRACE)) {
        switch (event.type) {
            case dfx::websocket::WebSocketEventType::ERROR_EVENT: {
                cloudLog(CLOUD_LOG_LEVEL_TRACE,
                         "CloudWebSocketJson::handleEvent(ERROR) received %d, %s\n",
                         event.error.code,
                         event.error.message.c_str());
                break;
            }
            case dfx::websocket::WebSocketEventType::OPEN: {
                cloudLog(CLOUD_LOG_LEVEL_TRACE, "CloudWebSocketJson::handleEvent(OPEN) received\n");
                break;
            }
            case dfx::websocket::WebSocketEventType::LISTEN: {
                cloudLog(CLOUD_LOG_LEVEL_TRACE, "CloudWebSocketJson::handleEvent(LISTEN) received\n");
                break;
            }
            case dfx::websocket::WebSocketEventType::CONNECTION: {
                cloudLog(CLOUD_LOG_LEVEL_TRACE, "CloudWebSocketJson::handleEvent(CONNECTION) received\n");
                break;
            }
            case dfx::websocket::WebSocketEventType::MESSAGE: {
                cloudLog(CLOUD_LOG_LEVEL_TRACE, "CloudWebSocketJson::handleEvent(MESSAGE) received\n");
                break;
            }
            case dfx::websocket::WebSocketEventType::CLOSED: {
                cloudLog(CLOUD_LOG_LEVEL_TRACE, "CloudWebSocketJson::handleEvent(CLOSE) received\n");
                break;
            }
            default: {
                cloudLog(CLOUD_LOG_LEVEL_TRACE,
                         "CloudWebSocketJson::handleEvent(UNHANDLED) received %d, %s\n",
                         event.error.code,
                         event.error.message.c_str());
            }
        }
    }
#endif // NDEBUG

    switch (event.type) {
        case dfx::websocket::WebSocketEventType::ERROR_EVENT: {
            cloudLog(CLOUD_LOG_LEVEL_ERROR, "SOCKET ERROR EVENT --- SHUTTING DOWN\n");
            {
                std::unique_lock<std::mutex> lock(mutex);
                closedReason = event.error.message; // Keep the reason it was closed
                closed = true;
            }

            // Need to shut down if we have trouble opening
            {
                // If this happened on open, wake up that original request
                std::unique_lock<std::mutex> lock(mutexOpen);
                cvWebSocketOpen.notify_all();
            }

            {
                // Wake up any client threads pending on response
                std::unique_lock<std::mutex> lock(mutex);
                responses.clear();
                for (auto& condition : pending) {
                    condition.second->notify_all();
                }
                pending.clear(); // Remove all now that they have been notified
            }

            break;
        }
        case dfx::websocket::WebSocketEventType::OPEN: {
            std::unique_lock<std::mutex> lock(mutexOpen);
            cvWebSocketOpen.notify_all(); // Wake up thread that was waiting on the open
            break;
        }
        case dfx::websocket::WebSocketEventType::LISTEN:
        case dfx::websocket::WebSocketEventType::CONNECTION:
            break;
        case dfx::websocket::WebSocketEventType::MESSAGE:
            handleMessageEvent(event.message);
            break;
        case dfx::websocket::WebSocketEventType::CLOSED: {
            std::unique_lock<std::mutex> lock(mutex); // Protect pending, closed
            closedReason = "received closed";
            closed = true;
            responses.clear();
            for (const auto& condition : pending) {
                condition.second->notify_all();
            }
            pending.clear(); // Remove all now that they have been notified
            break;
        }
    }
}

void CloudWebSocketJson::handleMessageEvent(const dfx::websocket::WebSocketMessageEvent& messageEvent)
{
    if (messageEvent.data == nullptr) {
        cloudLog(CLOUD_LOG_LEVEL_WARNING, "Ignoring empty data event\n");
    } else {
        auto rawData = reinterpret_cast<const char*>(messageEvent.data->data());
        std::string requestID(rawData, 10);

        std::unique_lock<std::mutex> lock(mutex); // Protect pending
        if (requestID.rfind("STRM") != 0) {
            // This is not a stream response so wake up client and send them the response
            auto iter = pending.find(requestID);
            if (iter != pending.end()) {
                auto condition = iter->second;
                pending.erase(iter);

                responses[requestID] = messageEvent.data;

                condition->notify_one();
            } else {
                // Something bad has happened - shutdown
                if (cloudLogIsActive(CLOUD_LOG_LEVEL_ERROR)) {
                    cloudLog(CLOUD_LOG_LEVEL_ERROR, "requestID %s missing, nothing to notify\n", requestID.c_str());

                    auto hexData = dfx::api::utils::hexDump(
                        "Unhandled Message", messageEvent.data.get()->data(), messageEvent.data->size());
                    cloudLog(CLOUD_LOG_LEVEL_ERROR, "%s", hexData.c_str());
                }
            }
        } else {
            auto iter = streams.find(requestID);
            if (iter != streams.end()) {
                iter->second->handleStreamResponse(messageEvent.data);
            }
        }
    }
}

// The header comprises 4 digits for the actionID followed by a 10 digit requestID. In order
// to track which type of request it was when we receive a response, the first four of the 10 digit
// requestID stores the actionID leaving the remaining 6 digits to represent a monotonically
// increasing number. When it reaches the maximum number representable by those 6 digits, it is
// wrapped back to the start.
std::string CloudWebSocketJson::getRequestID(int actionID)
{
    auto nextID = lastTransactionID++;
    if (nextID > 999999) {
        lastTransactionID = 2;
        nextID = 1;
    }
    assert(actionID <= 9999);              // Developer error, no actionID > 9999.
    actionID = (std::min)(actionID, 9999); // Explicit (std::min) to help Windows compile.
    auto requestID = fmt::format("{:04}{:06}", actionID, nextID);
    return requestID;
}

void CloudWebSocketJson::registerStream(const std::string& streamID, MeasurementStreamWebSocketJson* measurementStream)
{
    streams[streamID] = measurementStream;
}

void CloudWebSocketJson::deregisterStream(const std::string& streamID)
{
    auto iter = streams.find(streamID);
    if (iter != streams.end()) {
        streams.erase(iter);
    }
}

CloudStatus CloudWebSocketJson::sendMessageJson(const CloudConfig& config,
                                                const dfx::api::web::WebServiceDetail& detail,
                                                const nlohmann::json& params,
                                                const nlohmann::json& query,
                                                const nlohmann::json& message,
                                                nlohmann::json& response)
{
    nlohmann::json request = message;
    nlohmann::json requestParams = params;

    if (!query.empty()) {
        request["Query"] = query;
    }

    if (config.listLimit > 0) {
        requestParams["Limit"] = config.listLimit;
    }
    if (!requestParams.empty()) {
        request["Params"] = requestParams;
    }

    std::string requestString;
    if (!request.empty()) {  // avoid "null" requestString
        requestString = to_string(request);
    } else {
        requestString = "{}";
    }

    auto actionID = fmt::format("{:04}", detail.wsCode);
    auto requestID = getRequestID(detail.wsCode);

    auto strRequest = actionID + requestID + requestString;

    auto condition = std::make_shared<std::condition_variable>();
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (webSocket->getState() != WebSocketState::OPEN) {
            return CloudStatus(CLOUD_TRANSPORT_CLOSED, closedReason);
        }
        pending[requestID] = condition;

#ifndef NDEBUG
        if (cloudLogIsActive(CLOUD_LOG_LEVEL_DEBUG)) {
            cloudLog(CLOUD_LOG_LEVEL_DEBUG,
                     "Request [%d,%s,%s]\n",
                     detail.wsCode,
                     detail.httpOption.c_str(),
                     detail.urlPath.c_str());
            if (cloudLogIsActive(CLOUD_LOG_LEVEL_TRACE)) {
                auto hexBytes = dfx::api::utils::hexDump("Request bytes:\n", strRequest.c_str(), strRequest.size());
                cloudLog(CLOUD_LOG_LEVEL_TRACE, "%s", hexBytes.c_str());

                // Convert the message data to a string and parse it as JSON to dump it
                if (strRequest.size() >= 15) {
                    std::string identifier = strRequest.substr(0, 14);
                    std::string jsonPayload = strRequest.substr(14, strRequest.size() - 14);
                    nlohmann::json jsonData = nlohmann::json::parse(jsonPayload, nullptr, false);
                    std::string jsonString = jsonData.dump(4);
                    cloudLog(CLOUD_LOG_LEVEL_TRACE, "Request JSON:\n%s\n%s\n", identifier.c_str(), jsonString.c_str());
                }
            }
        }
#endif

        // Need to check we are not terminated before the send and wait
        webSocket->sendUTF8(strRequest);

        condition->wait(lock);
        if (closed) {
            return CloudStatus(CLOUD_TRANSPORT_CLOSED, closedReason);
        }
    }

    auto iter = responses.find(requestID);
    if (iter == responses.end()) {
        // Unable to find a response for the requestID, should not happen
        return CloudStatus(CLOUD_INTERNAL_ERROR);
    } else {
        auto message = iter->second;
        responses.erase(iter);

        if (message->size() < PAYLOAD_OFFSET) {
            return CloudStatus(CLOUD_INTERNAL_ERROR,
                               "Response message was too small: " + std::to_string(message->size()));
        }

        auto rawData = reinterpret_cast<const char*>(message->data());

        std::string statusCode(rawData + 10, 3);

        auto rawSize = message->size();
        assert(rawSize < INT_MAX); // Explicit cast - something wrong if this big
        int messageSize = static_cast<int>(rawSize);

#ifndef NDEBUG
        if (cloudLogIsActive(CLOUD_LOG_LEVEL_DEBUG)) {
            cloudLog(CLOUD_LOG_LEVEL_DEBUG,
                     "Response [%d,%s,%s]\n",
                     detail.wsCode,
                     detail.httpOption.c_str(),
                     detail.urlPath.c_str());
            if (cloudLogIsActive(CLOUD_LOG_LEVEL_TRACE)) {
                auto hexBytes = dfx::api::utils::hexDump("Response bytes:\n", message->data(), message->size());
                cloudLog(CLOUD_LOG_LEVEL_TRACE, "%s", hexBytes.c_str());

                // Convert the message data to a string and parse it as JSON to dump it
                if (message->size() >= 15) {
                    auto data = reinterpret_cast<char*>(message->data());
                    std::string messageString(data, message->size());
                    std::string identifier = messageString.substr(0, 13);
                    std::string jsonPayload = messageString.substr(13, message->size() - 13);
                    nlohmann::json jsonData = nlohmann::json::parse(jsonPayload, nullptr, false);
                    std::string jsonString = jsonData.dump(4);
                    cloudLog(CLOUD_LOG_LEVEL_TRACE, "Response JSON:\n%s\n%s\n", identifier.c_str(), jsonString.c_str());
                }
            }
        }
#endif

        std::string responseData(rawData + PAYLOAD_OFFSET, messageSize - PAYLOAD_OFFSET);
        response = nlohmann::json::parse(responseData, nullptr, false);

        // Request is considered OK
        if (statusCode == "200") {
            if (!response.is_discarded()) {
                return CloudStatus(CLOUD_OK);
            }
        }

        if (statusCode == "400") {
            auto errorCode = response["Code"].get<std::string>();
            auto errorMessage = response["Message"].get<std::string>();
            if (errorCode == "INCORRECT_REQUEST" && errorMessage.substr(0, 20) == "Invalid Route Number") {
                // Cloud is reporting it an unsupported feature
                return CloudStatus(CLOUD_UNSUPPORTED_FEATURE, errorMessage);
            }
            return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR);
        } else if (statusCode == "403") {
            return CloudStatus(CLOUD_USER_NOT_AUTHORIZED);
        } else if (statusCode == "404") {
            return CloudStatus(CLOUD_RECORD_NOT_FOUND);
        } else if (statusCode == "409") {
            return CloudStatus(CLOUD_RECORD_ALREADY_EXISTS);
        } else if (statusCode == "500") {
            return CloudStatus(CLOUD_INTERNAL_ERROR);
        }
        return CloudStatus(CLOUD_INTERNAL_ERROR);
    }
}

CloudStatus CloudWebSocketJson::login(CloudConfig& config)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, login(config));

    nlohmann::json request;
    request["Email"] = config.authEmail;
    request["Password"] = config.authPassword;

    if (!config.authMFAToken.empty()) {
        request["MFAToken"] = config.authMFAToken;
    }

    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/users/login
    auto result = this->sendMessageJson(config, web::Users::Login, {}, {}, request, response);

    if (result.OK()) {
        config.authToken = response["Token"];

        if (response.contains("RefreshToken")) {
            config.userRefreshToken = response["RefreshToken"];
        }
    }

    return result;
}

CloudStatus CloudWebSocketJson::loginWithToken(CloudConfig& config, std::string& token)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, loginWithToken(config, token));

    nlohmann::json request = {{"Token", token}};
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/organizations/login-with-token
    auto result = this->sendMessageJson(config, web::Organizations::LoginWithToken, {}, {}, request, response);
    if (result.OK()) {
        token = response["Token"];
    }

    return result;
}

// WebSocketJSON does not have a logout for users - so calling can't fail
CloudStatus CloudWebSocketJson::logout(CloudConfig& config)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, logout(config));
    return CloudStatus(CLOUD_OK);
}

CloudStatus CloudWebSocketJson::registerDevice(CloudConfig& config,
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
    auto result = this->sendMessageJson(config, web::Organizations::RegisterLicense, {}, {}, request, response);

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

CloudStatus CloudWebSocketJson::unregisterDevice(CloudConfig& config)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, unregisterDevice(config));

    nlohmann::json request;
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/organizations/unregister-license
    auto result = this->sendMessageJson(config, web::Organizations::UnregisterLicense, {}, {}, request, response);
    return result;
}

CloudStatus CloudWebSocketJson::verifyToken(const CloudConfig& config, std::string& response)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, verifyToken(config, response));

    nlohmann::json request;
    nlohmann::json jsonResponse;

    auto result = this->sendMessageJson(config, web::General::VerifyToken, {}, {}, request, jsonResponse);
    if (result.OK()) {
        response = jsonResponse.dump();
    }
    return result;
}

CloudStatus CloudWebSocketJson::renewToken(const CloudConfig& config, std::string& token, std::string& refreshToken)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, renew(config, token, refreshToken));

    nlohmann::json request = {{"Token", token}, {"RefreshToken", refreshToken}};
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/auths/renew
    auto result = this->sendMessageJson(config, web::Auths::RenewToken, {}, {}, request, response);
    if (result.OK()) {
        token = response["Token"];
        refreshToken = response["RefreshToken"];
    }

    return result;
}

CloudStatus CloudWebSocketJson::switchEffectiveOrganization(CloudConfig& config, const std::string& organizationID)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE);
}

std::shared_ptr<DeviceAPI> CloudWebSocketJson::device(const CloudConfig& config)
{
    return std::make_shared<DeviceWebSocketJson>(config,
                                                 std::static_pointer_cast<CloudWebSocketJson>(shared_from_this()));
}

std::shared_ptr<LicenseAPI> CloudWebSocketJson::license(const CloudConfig& config)
{
    return std::make_shared<LicenseWebSocketJson>(config,
                                                  std::static_pointer_cast<CloudWebSocketJson>(shared_from_this()));
}

std::shared_ptr<MeasurementAPI> CloudWebSocketJson::measurement(const CloudConfig& config)
{
    return std::make_shared<MeasurementWebSocketJson>(config,
                                                      std::static_pointer_cast<CloudWebSocketJson>(shared_from_this()));
}

std::shared_ptr<MeasurementStreamAPI> CloudWebSocketJson::measurementStream(const CloudConfig& config)
{
    return std::make_shared<MeasurementStreamWebSocketJson>(
        config, std::static_pointer_cast<CloudWebSocketJson>(shared_from_this()));
}

std::shared_ptr<OrganizationAPI> CloudWebSocketJson::organization(const CloudConfig& config)
{
    return std::make_shared<OrganizationWebSocketJson>(
        config, std::static_pointer_cast<CloudWebSocketJson>(shared_from_this()));
}

std::shared_ptr<ProfileAPI> CloudWebSocketJson::profile(const CloudConfig& config)
{
    return std::make_shared<ProfileWebSocketJson>(config,
                                                  std::static_pointer_cast<CloudWebSocketJson>(shared_from_this()));
}

std::shared_ptr<StudyAPI> CloudWebSocketJson::study(const CloudConfig& config)
{
    return std::make_shared<StudyWebSocketJson>(config,
                                                std::static_pointer_cast<CloudWebSocketJson>(shared_from_this()));
}

std::shared_ptr<UserAPI> CloudWebSocketJson::user(const CloudConfig& config)
{
    return std::make_shared<UserWebSocketJson>(config,
                                               std::static_pointer_cast<CloudWebSocketJson>(shared_from_this()));
}

const std::string& CloudWebSocketJson::getTransportType()
{
    return CloudAPI::TRANSPORT_TYPE_WEBSOCKET_JSON;
}

//     *	status			| Description
//     *	---------------	|-----------------------------
//     *	ACTIVE			| Online and fully operational
//     *	MAINTENANCE		| Offline for maintenance
//     *	ERROR			| Offline due to an error
//     *	LATENCY			| API is experience latency or a general slowdown
CloudStatus CloudWebSocketJson::getServerStatus(CloudConfig& config, std::string& response)
{
    nlohmann::json request;
    nlohmann::json jsonResponse;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/general/status
    auto result = this->sendMessageJson(config, web::General::Status, {}, {}, request, jsonResponse);
    if (result.OK()) {
        response = jsonResponse.dump();
    }
    return result;
}
