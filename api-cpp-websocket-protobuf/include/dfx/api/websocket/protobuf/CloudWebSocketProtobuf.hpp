// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_WEBSOCKET_PROTOBUF_H
#define DFX_API_CLOUD_WEBSOCKET_PROTOBUF_H

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
void CloudWebSocketProtobufCallback(const dfx::websocket::WebSocketEvent&, void*);
}

namespace dfx::api::websocket::protobuf
{
class MeasurementStreamWebSocketProtobuf;

/**
 * \class CloudWebSocketProtobuf CloudWebSocketProtobuf.h "dfx/api/websocket/protobuf/CloudWebSocketProtobuf.h"
 *
 * \brief CloudWebSocketProtobuf provides a C++ class which communicates with the Nuralogix
 *        DFX API version 2 servers using WebSocket Protobuf communications.
 */
class CloudWebSocketProtobuf : public CloudAPI
{
public:
    explicit CloudWebSocketProtobuf(const CloudConfig& config);

    ~CloudWebSocketProtobuf() override;

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
    friend void ::CloudWebSocketProtobufCallback(const dfx::websocket::WebSocketEvent&, void*);
    friend class DeviceWebSocketProtobuf;
    friend class LicenseWebSocketProtobuf;
    friend class MeasurementWebSocketProtobuf;
    friend class MeasurementStreamWebSocketProtobuf;
    friend class ProfileWebSocketProtobuf;
    friend class StudyWebSocketProtobuf;
    friend class UserWebSocketProtobuf;
    friend class OrganizationWebSocketProtobuf;

    CloudStatus sendMessage(const dfx::api::web::WebServiceDetail& detail,
                            ::google::protobuf::Message& message,
                            ::google::protobuf::Message& response);
    void handleEvent(const dfx::websocket::WebSocketEvent& event);
    void handleMessageEvent(const dfx::websocket::WebSocketMessageEvent& messageEvent);
    std::string getRequestID(int actionID);

    CloudStatus decodeWebSocketError(const std::string& statusCode, const std::vector<uint8_t>& data);

    void registerStream(const std::string& streamID, MeasurementStreamWebSocketProtobuf* measurementStream);
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
    std::map<std::string, MeasurementStreamWebSocketProtobuf*> streams;
    std::map<std::string, std::shared_ptr<std::condition_variable>> pending;
    std::map<std::string, std::shared_ptr<std::vector<uint8_t>>> responses;
};

} // namespace dfx::api::websocket::protobuf

#endif // DFX_API_CLOUD_WEBSOCKET_PROTOBUF_H
