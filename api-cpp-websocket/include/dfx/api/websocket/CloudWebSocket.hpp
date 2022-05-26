// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_WEBSOCKET_API_H
#define DFX_API_CLOUD_WEBSOCKET_API_H

#include "dfx/api/CloudAPI.hpp"
#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/web/WebServiceDetail.hpp"
#include "dfx/websocket/WebSocket.hpp"

#include <condition_variable>
#include <google/protobuf/message.h>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>

extern "C" {
void CloudWebSocketCallback(const dfx::websocket::WebSocketEvent&, void*);
}

namespace dfx::api::websocket
{
class MeasurementStreamWebSocket;

/**
 * \class CloudWebSocket CloudWebSocket.h "dfx/api/websocket/CloudWebSocket.h"
 *
 * \brief CloudWebSocketAPI provides a C++ class which communicates with the Nuralogix
 *        DFX API version 2 servers using WebSocket communications.
 */
class CloudWebSocket : public CloudAPI
{
public:
    explicit CloudWebSocket(const CloudConfig& config);

    ~CloudWebSocket() override;

    CloudStatus connect(const CloudConfig& config) override;

    const std::string& getTransportType() override;

    CloudStatus getServerStatus(CloudConfig& config) override;

    // *********************************************************************************
    // AUTHENTICATION SECTION
    // *********************************************************************************

    CloudStatus login(CloudConfig& config) override;

    CloudStatus logout(CloudConfig& config) override;

    CloudStatus registerDevice(CloudConfig& config, const std::string& appName, const std::string& appVersion) override;

    CloudStatus unregisterDevice(CloudConfig& config) override;

    CloudStatus validateToken(const CloudConfig& config, const std::string& userToken) override;

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
    friend void ::CloudWebSocketCallback(const dfx::websocket::WebSocketEvent&, void*);
    friend class DeviceWebSocket;
    friend class LicenseWebSocket;
    friend class MeasurementWebSocket;
    friend class MeasurementStreamWebSocket;
    friend class ProfileWebSocket;
    friend class SignalWebSocket;
    friend class StudyWebSocket;
    friend class UserWebSocket;
    friend class OrganizationWebSocket;

    CloudStatus sendMessage(const dfx::api::web::WebServiceDetail& detail,
                            ::google::protobuf::Message& message,
                            ::google::protobuf::Message& response);
    void handleEvent(const dfx::websocket::WebSocketEvent& event);
    void handleMessageEvent(const dfx::websocket::WebSocketMessageEvent& messageEvent);
    std::string getRequestID(int actionID);

    CloudStatus decodeWebSocketError(const std::string& statusCode, const std::vector<uint8_t>& data);

    void registerStream(const std::string& streamID, MeasurementStreamWebSocket* measurementStream);
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
    std::map<std::string, MeasurementStreamWebSocket*> streams;
    std::map<std::string, std::shared_ptr<std::condition_variable>> pending;
    std::map<std::string, std::shared_ptr<std::vector<uint8_t>>> responses;
};

} // namespace dfx::api::websocket

#endif // DFX_API_CLOUD_WEBSOCKET_API_H
