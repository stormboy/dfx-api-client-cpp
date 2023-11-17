// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_WEBSOCKET_JSON_API_H
#define DFX_API_CLOUD_WEBSOCKET_JSON_API_H

#include "dfx/api/CloudAPI.hpp"
#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/web/WebServiceDetail.hpp"
#include "dfx/websocket/WebSocket.hpp"

#include <condition_variable>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>

extern "C" {
void CloudWebSocketJsonCallback(const dfx::websocket::WebSocketEvent&, void*);
}

namespace dfx::api::websocket::json
{
class MeasurementStreamWebSocketJson;

/**
 * \class CloudWebSocketJson CloudWebSocketJson.h "dfx/api/websocketjson/CloudWebSocketJson.h"
 *
 * \brief CloudWebSocketJson provides a C++ class which communicates with the Nuralogix
 *        DFX API version 2 servers using JSON over a WebSocket communication.
 */
class CloudWebSocketJson : public CloudAPI
{
public:
    explicit CloudWebSocketJson(const CloudConfig& config);

    ~CloudWebSocketJson() override;

    CloudStatus connect(const CloudConfig& config) override;

    const std::string& getTransportType() override;

    CloudStatus getServerStatus(CloudConfig& config, std::string& response) override;

    // *********************************************************************************
    // AUTHENTICATION SECTION
    // *********************************************************************************

    CloudStatus login(CloudConfig& config) override;

    CloudStatus loginWithToken(CloudConfig& config, std::string& token) override;

    CloudStatus logout(CloudConfig& config) override;

    CloudStatus registerDevice(CloudConfig& config,
                               const std::string& appName,
                               const std::string& appVersion,
                               const uint16_t tokenExpiresInSeconds,
                               const std::string& tokenSubject) override;

    CloudStatus unregisterDevice(CloudConfig& config) override;

    CloudStatus verifyToken(const CloudConfig& config, std::string& response) override;

    CloudStatus renewToken(const CloudConfig& config, std::string& token, std::string& refreshToken) override;

    CloudStatus switchEffectiveOrganization(CloudConfig& config, const std::string& organizationID) override;

    // *********************************************************************************
    // SERVICES SECTION
    // *********************************************************************************

    std::shared_ptr<DeviceAPI> device(const CloudConfig& config) override;

    std::shared_ptr<LicenseAPI> license(const CloudConfig& config) override;

    std::shared_ptr<MeasurementAPI> measurement(const CloudConfig& config) override;

    std::shared_ptr<MeasurementStreamAPI> measurementStream(const CloudConfig& config) override;

    std::shared_ptr<OrganizationAPI> organization(const CloudConfig& config) override;

    std::shared_ptr<ProfileAPI> profile(const CloudConfig& config) override;

    std::shared_ptr<StudyAPI> study(const CloudConfig& config) override;

    std::shared_ptr<UserAPI> user(const CloudConfig& config) override;

private:
    friend void ::CloudWebSocketJsonCallback(const dfx::websocket::WebSocketEvent&, void*);
    friend class DeviceWebSocketJson;
    friend class LicenseWebSocketJson;
    friend class MeasurementWebSocketJson;
    friend class MeasurementStreamWebSocketJson;
    friend class ProfileWebSocketJson;
    friend class StudyWebSocketJson;
    friend class UserWebSocketJson;
    friend class OrganizationWebSocketJson;

    CloudStatus sendMessageJson(const CloudConfig& config,
                                const dfx::api::web::WebServiceDetail& detail,
                                const nlohmann::json& params,
                                const nlohmann::json& query,
                                const nlohmann::json& message,
                                nlohmann::json& response);

    void handleEvent(const dfx::websocket::WebSocketEvent& event);
    void handleMessageEvent(const dfx::websocket::WebSocketMessageEvent& messageEvent);
    std::string getRequestID(int actionID);

    void registerStream(const std::string& streamID, MeasurementStreamWebSocketJson* measurementStream);
    void deregisterStream(const std::string& streamID);

private:
    const int PAYLOAD_OFFSET = 13;
    std::string closedReason;
    bool closed;
    std::mutex mutexOpen;
    std::condition_variable cvWebSocketOpen;

    std::shared_ptr<dfx::websocket::WebSocket> webSocket;
    std::atomic<int> lastTransactionID;
    std::mutex mutex;
    std::condition_variable cvServiceThread;
    std::map<std::string, MeasurementStreamWebSocketJson*> streams;
    std::map<std::string, std::shared_ptr<std::condition_variable>> pending;
    std::map<std::string, std::shared_ptr<std::vector<uint8_t>>> responses;
};

} // namespace dfx::api::websocket::json

#endif // DFX_API_CLOUD_WEBSOCKET_JSON_API_H
