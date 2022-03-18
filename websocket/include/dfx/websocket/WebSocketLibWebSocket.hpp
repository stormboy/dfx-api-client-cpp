// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFXAPI_WEBSOCKETLIBWEBSOCKET_HPP
#define DFXAPI_WEBSOCKETLIBWEBSOCKET_HPP

#include "dfx/websocket/WebSocket.hpp"

#include <condition_variable>
#include <cstdint>
#include <deque>
#include <string>
#include <thread>
#include <vector>

#include <libwebsockets.h>

namespace dfx::websocket
{

class WebSocketLibWebSocket : public WebSocket
{
public:
    WebSocketLibWebSocket(int logLevel, LogCallback callback);

    ~WebSocketLibWebSocket() override;

    void setLogLevel(uint8_t level, LogCallback function) override;

    void setRootCertificate(const std::string& rootCA) override;

    void open(const std::string& inputURL, const std::string& protocol) override;

    void close() override;

    void sendUTF8(const std::string& data) override;

    void sendBinary(const std::vector<uint8_t>& data) override;

    static int dfx_wss_callback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len);
    static void dfx_wss_log_emit(int level, const char* line);

    static void
    dfx_wss_log_callback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len);

private:
    void sendFormattedBinaryBuffer(std::shared_ptr<std::vector<uint8_t>> buffer);

    int handleEvent(struct lws* wsi, enum lws_callback_reasons reason, void* in, size_t len);

    void serviceThreadRunnable();

    std::mutex mutex;
    std::unique_ptr<std::thread> serviceThread;

    std::mutex shutdownMutex;
    bool serviceThreadJoined;

    std::string certFilePath;

    std::deque<std::shared_ptr<std::vector<uint8_t>>> pendingSendData;
    struct lws_context* context;
    struct lws* wsi;
};

} // namespace dfx::websocket

#endif // DFXAPI_WEBSOCKETLIBWEBSOCKET_HPP
