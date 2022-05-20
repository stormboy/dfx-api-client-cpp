// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/websocket/CloudWebSocket.hpp"
#include "dfx/api/CloudLog.hpp"

#include "dfx/api/utils/HexDump.hpp"
#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/websocket/DeviceWebSocket.hpp"
#include "dfx/api/websocket/LicenseWebSocket.hpp"
#include "dfx/api/websocket/MeasurementStreamWebSocket.hpp"
#include "dfx/api/websocket/MeasurementWebSocket.hpp"
#include "dfx/api/websocket/OrganizationWebSocket.hpp"
#include "dfx/api/websocket/ProfileWebSocket.hpp"
#include "dfx/api/websocket/StudyWebSocket.hpp"
#include "dfx/api/websocket/UserWebSocket.hpp"

#include "dfx/api/web/WebServiceDetail.hpp"

#include "dfx/websocket/WebSocket.hpp"

#include "dfx/proto/general.pb.h"
#include "dfx/proto/organizations.pb.h"
#include "dfx/proto/users.pb.h"
#include "dfx/proto/util.pb.h"

#include "fmt/format.h"
#include "nlohmann/json.hpp"

#include <sstream>
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

#include "dfx/api/utils/FileUtils.hpp"
#include <algorithm>
#include <chrono>
#include <thread>
using namespace dfx::api;
using namespace dfx::api::websocket;
using namespace dfx::websocket;

extern "C" {
void CloudWebSocketCallback(const WebSocketEvent& event, void* userData)
{
    auto self = static_cast<CloudWebSocket*>(userData);
    self->handleEvent(event);
}
}

CloudWebSocket::CloudWebSocket(const CloudConfig& config)
    : CloudAPI(config), lastTransactionID(1), closedReason(""), closed(true)
{
}

CloudStatus CloudWebSocket::connect(const CloudConfig& config)
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
    webSocket->setEventCallback(&CloudWebSocketCallback, this);

    std::string wssURL = fmt::format("wss://{}:{}", config.serverHost, config.serverPort);
    std::string wssProtocol("proto");
    webSocket->open(wssURL, wssProtocol);

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

CloudWebSocket::~CloudWebSocket()
{
    // The webSocket thread may notify us via a callback so we need to ensure the
    // webSocket is properly closed and its associated thread terminates prior to
    // allowing the memory associated with this instance from being released.
    if (webSocket != nullptr) {
        webSocket->close();
    }
}

void CloudWebSocket::handleEvent(const WebSocketEvent& event)
{
#ifndef NDEBUG
    if (cloudLogIsActive(CLOUD_LOG_LEVEL_TRACE)) {
        switch (event.type) {
            case dfx::websocket::WebSocketEventType::ERROR_EVENT: {
                cloudLog(CLOUD_LOG_LEVEL_TRACE,
                         "CloudWebSocket::handleEvent(ERROR) received %d, %s\n",
                         event.error.code,
                         event.error.message.c_str());
                break;
            }
            case dfx::websocket::WebSocketEventType::OPEN: {
                cloudLog(CLOUD_LOG_LEVEL_TRACE, "CloudWebSocket::handleEvent(OPEN) received\n");
                break;
            }
            case dfx::websocket::WebSocketEventType::LISTEN: {
                cloudLog(CLOUD_LOG_LEVEL_TRACE, "CloudWebSocket::handleEvent(LISTEN) received\n");
                break;
            }
            case dfx::websocket::WebSocketEventType::CONNECTION: {
                cloudLog(CLOUD_LOG_LEVEL_TRACE, "CloudWebSocket::handleEvent(CONNECTION) received\n");
                break;
            }
            case dfx::websocket::WebSocketEventType::MESSAGE: {
                cloudLog(CLOUD_LOG_LEVEL_TRACE, "CloudWebSocket::handleEvent(MESSAGE) received\n");
                break;
            }
            case dfx::websocket::WebSocketEventType::CLOSED: {
                cloudLog(CLOUD_LOG_LEVEL_TRACE, "CloudWebSocket::handleEvent(CLOSE) received\n");
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

#ifdef HAVE_DECODER_SUPPORT
#include "dfx/proto/results.pb.h"
#include <google/protobuf/util/json_util.h>
#endif

void CloudWebSocket::handleMessageEvent(const dfx::websocket::WebSocketMessageEvent& messageEvent)
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
std::string CloudWebSocket::getRequestID(int actionID)
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

void CloudWebSocket::registerStream(const std::string& streamID, MeasurementStreamWebSocket* measurementStream)
{
    streams[streamID] = measurementStream;
}

void CloudWebSocket::deregisterStream(const std::string& streamID)
{
    auto iter = streams.find(streamID);
    if (iter != streams.end()) {
        streams.erase(iter);
    }
}

CloudStatus CloudWebSocket::sendMessage(const dfx::api::web::WebServiceDetail& detail,
                                        ::google::protobuf::Message& message,
                                        ::google::protobuf::Message& response)
{
    std::string requestString;
    if (!message.SerializeToString(&requestString)) {
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR);
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
            }
        }
#endif

        // Request is considered OK
        if (statusCode == "200") {
            if (response.ParseFromArray(rawData + PAYLOAD_OFFSET, messageSize - PAYLOAD_OFFSET)) {
                return CloudStatus(CLOUD_OK);
            } else {
                return CloudStatus(CLOUD_INTERNAL_ERROR);
            }
        } else if (statusCode == "403") {
            if (response.GetTypeName() == "dfx.proto.users.LoginResponse") {
                if (response.ParseFromArray(rawData + PAYLOAD_OFFSET, messageSize - PAYLOAD_OFFSET)) {
                    dfx::proto::users::LoginResponse* loginResponse =
                        static_cast<dfx::proto::users::LoginResponse*>(&response);
                    return CloudStatus(CLOUD_USER_NOT_AUTHORIZED, loginResponse->token());
                }
            }
        } else if (statusCode == "409") {
            if (response.ParseFromArray(rawData + PAYLOAD_OFFSET, messageSize - PAYLOAD_OFFSET)) {
                // Don't really know what type of response it is, best we can do is grab the debug string
                return CloudStatus(CLOUD_RECORD_ALREADY_EXISTS, response.DebugString());
            }
        } else if (statusCode == "500") {
            if (response.ParseFromArray(rawData + PAYLOAD_OFFSET, messageSize - PAYLOAD_OFFSET)) {
                // Don't really know what type of response it is, best we can do is grab the debug string
                // which returns something like: "DeviceID: \"INTERNAL_ERROR\"\n"
                return CloudStatus(CLOUD_INTERNAL_ERROR, response.DebugString());
            }
        }

        // Some form of error occurred
        return decodeWebSocketError(statusCode, std::vector<uint8_t>(rawData + PAYLOAD_OFFSET, rawData + messageSize));
    }
}

CloudStatus CloudWebSocket::decodeWebSocketError(const std::string& statusCode, const std::vector<uint8_t>& data)
{
    dfx::proto::util::Error error;
    if (error.ParseFromArray(data.data(), data.size())) {
        // We have the error, build up the error string from the proto msgs.

        // https://dfxapiversion10.docs.apiary.io/#introduction/3.-using-api-transports
        if (statusCode == "400") { // VALIDATION_ERROR
            std::stringstream errResponse;
            errResponse << "Validation error on fields: ";
            int index = 0;
            for (auto& err : error.errors()) {
                errResponse << err.first;
                if (index++ < error.errors_size() - 1) {
                    errResponse << ", ";
                } else {
                    errResponse << ".";
                }
            }
            return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, errResponse.str());
        }

        if (statusCode == "404") {
            return CloudStatus(CLOUD_RECORD_NOT_FOUND);
        }

        if (statusCode == "403") {
            return CloudStatus(CLOUD_USER_NOT_AUTHORIZED);
        }

        if (cloudLogIsActive(CLOUD_LOG_LEVEL_ERROR)) {
            std::stringstream errResponse;
            errResponse << "StatusCode: " << statusCode << "\n";
            errResponse << "Error: " << error.code() << "\n";
            errResponse << "Message: " << error.message() << "\n";
            for (auto& err : error.errors()) {
                errResponse << "Err: " << err.first << "\n";
                auto description = err.second;
                for (auto& msg : description.msgs()) {
                    errResponse << "\t" << msg << "\n";
                }
            }
            std::string errMessage = errResponse.str();
            cloudLog(CLOUD_LOG_LEVEL_ERROR, "WEB: Message status not handled!\n%s\n", errMessage.c_str());
        }
        return CloudStatus(CLOUD_INTERNAL_ERROR);

    } else {
        cloudLog(CLOUD_LOG_LEVEL_ERROR, "WEB: Message failed decode!\n");
        return CloudStatus(CLOUD_INTERNAL_ERROR);
    }
}

CloudStatus CloudWebSocket::login(CloudConfig& config)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, login(config));

    dfx::proto::users::LoginRequest request;
    request.set_email(config.authEmail);
    request.set_password(config.authPassword);

    dfx::proto::users::LoginResponse response;
    auto status = sendMessage(dfx::api::web::Users::Login, request, response);
    if (status.OK()) {
        config.authToken = response.token();
    }

    return status;
}

CloudStatus CloudWebSocket::logout(CloudConfig& config)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, logout(config));

    dfx::proto::organizations::UnregisterLicenseRequest request;
    request.set_deviceid(config.deviceID);
    request.set_key(config.license);

    dfx::proto::organizations::UnregisterLicenseResponse response;
    auto status = sendMessage(dfx::api::web::Organizations::UnregisterLicense, request, response);
    if (status.OK()) {
        config.deviceID = "";
        config.deviceToken = "";
        config.authToken = "";
    }
    return status;
}

CloudStatus CloudWebSocket::registerDevice(CloudConfig& config,
                                           const std::string& appName,
                                           const std::string& appVersion)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, registerDevice(config, appName, appVersion));

    dfx::proto::organizations::RegisterLicenseRequest request;
    request.set_key(config.license);
    auto deviceType = CloudAPI::getPlatform();
    deviceType = "LINUX";
    request.set_devicetypeid(deviceType);
    request.set_identifier(CloudAPI::getClientIdentifier());
    request.set_name(appName);
    request.set_version(appVersion);

    dfx::proto::organizations::RegisterLicenseResponse response;
    auto status = sendMessage(dfx::api::web::Organizations::RegisterLicense, request, response);
    if (status.OK()) {
        config.deviceID = response.deviceid();
        config.deviceToken = response.token();
    }
    return status;
}

CloudStatus CloudWebSocket::unregisterDevice(CloudConfig& config)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, unregisterDevice(config));
    dfx::proto::organizations::UnregisterLicenseRequest request;
    request.set_key(config.license);
    request.set_deviceid(config.deviceID);

    dfx::proto::organizations::UnregisterLicenseResponse response;
    return sendMessage(dfx::api::web::Organizations::UnregisterLicense, request, response);
}

CloudStatus CloudWebSocket::validateToken(const CloudConfig& config, const std::string& userToken)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE);
}

CloudStatus CloudWebSocket::switchEffectiveOrganization(CloudConfig& config, const std::string& organizationID)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE);
}

std::shared_ptr<DeviceAPI> CloudWebSocket::device(const CloudConfig& config)
{
    return std::make_shared<DeviceWebSocket>(config, std::static_pointer_cast<CloudWebSocket>(shared_from_this()));
}

std::shared_ptr<LicenseAPI> CloudWebSocket::license(const CloudConfig& config)
{
    return std::make_shared<LicenseWebSocket>(config, std::static_pointer_cast<CloudWebSocket>(shared_from_this()));
}

std::shared_ptr<MeasurementAPI> CloudWebSocket::measurement(const CloudConfig& config)
{
    return std::make_shared<MeasurementWebSocket>(config, std::static_pointer_cast<CloudWebSocket>(shared_from_this()));
}

std::shared_ptr<MeasurementStreamAPI> CloudWebSocket::measurementStream(const CloudConfig& config)
{
    return std::make_shared<MeasurementStreamWebSocket>(config,
                                                        std::static_pointer_cast<CloudWebSocket>(shared_from_this()));
}

std::shared_ptr<OrganizationAPI> CloudWebSocket::organization(const CloudConfig& config)
{
    return std::make_shared<OrganizationWebSocket>(config,
                                                   std::static_pointer_cast<CloudWebSocket>(shared_from_this()));
}

std::shared_ptr<ProfileAPI> CloudWebSocket::profile(const CloudConfig& config)
{
    return std::make_shared<ProfileWebSocket>(config, std::static_pointer_cast<CloudWebSocket>(shared_from_this()));
}

std::shared_ptr<StudyAPI> CloudWebSocket::study(const CloudConfig& config)
{
    return std::make_shared<StudyWebSocket>(config, std::static_pointer_cast<CloudWebSocket>(shared_from_this()));
}

std::shared_ptr<UserAPI> CloudWebSocket::user(const CloudConfig& config)
{
    return std::make_shared<UserWebSocket>(config, std::static_pointer_cast<CloudWebSocket>(shared_from_this()));
}

//     *	status			| Description
//     *	---------------	|-----------------------------
//     *	ACTIVE			| Online and fully operational
//     *	MAINTENANCE		| Offline for maintenance
//     *	ERROR			| Offline due to an error
//     *	LATENCY			| API is experience latency or a general slowdown
CloudStatus CloudWebSocket::getServerStatus(CloudConfig& config)
{
    dfx::proto::general::StatusRequest request;

    dfx::proto::general::StatusResponse response;
    auto status = sendMessage(dfx::api::web::General::Status, request, response);
    if (status.OK()) {
        auto version = response.version();
        auto status = response.statusid();
    }
    return status;
}
