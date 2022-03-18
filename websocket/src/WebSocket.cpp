// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/websocket/WebSocket.hpp"

#include <memory>

using dfx::websocket::WebSocket;
using dfx::websocket::WebSocketEvent;
using dfx::websocket::WebSocketState;

#ifndef __EMSCRIPTEN__
#include "dfx/websocket/WebSocketLibWebSocket.hpp"
#endif

std::shared_ptr<WebSocket> WebSocket::create(int logLevel, LogCallback callback)
{
#ifndef __EMSCRIPTEN__
    return std::make_shared<dfx::websocket::WebSocketLibWebSocket>(logLevel, callback);
#else
    return nullptr;
#endif
}

uint8_t WebSocket::logLevel = 0;
dfx::websocket::LogCallback WebSocket::logCallback = nullptr;

WebSocket::WebSocket() : state(WebSocketState::CREATED), onEventCallback(nullptr), eventUserData(nullptr) {}

void WebSocket::setState(WebSocketState state)
{
    std::unique_lock<std::mutex> lock(mutex);
    this->state = state;
}

WebSocketState WebSocket::getState()
{
    std::unique_lock<std::mutex> lock(mutex);
    return state;
}

// called by client thread
void dfx::websocket::WebSocket::setEventCallback(const WebSocketEventCallback& callback, void* userData)
{
    onEventCallback = callback;
    eventUserData = userData;

    if (callback) {
        std::unique_lock<std::mutex> lock(pendingEventsMutex); // Protect - pendingEvents
        // Drain pending events now that we have an event handler
        for (auto& event : pendingEvents) {
            onEventCallback(event, eventUserData);
        }
        pendingEvents.clear();
    }
}

// Called by websocket thread
void WebSocket::notifyClient(const WebSocketEvent& event)
{
    if (onEventCallback) {
        onEventCallback(event, eventUserData);
    } else {
        std::unique_lock<std::mutex> lock(pendingEventsMutex); // Protect - pendingEvents
        pendingEvents.push_back(event);
    }
}

void WebSocket::setLogLevel(uint8_t level, LogCallback function) {}

void WebSocket::setRootCertificate(const std::string& rootCA) {}

dfx::websocket::Status WebSocket::getEvent(WebSocketEvent& event, uint32_t /*timeout*/)
{
    // More elegant would be to use the timeout to wait until a result comes in
    // or timeout but hopefully this is sufficient.
    std::unique_lock<std::mutex> lock(pendingEventsMutex); // Protect - pendingEvents
    if (!pendingEvents.empty()) {
        event = pendingEvents.front();
        pendingEvents.pop_front();
    }
    return Status::OK;
}

void WebSocket::log(uint8_t level, const char* format, ...)
{
    if (WebSocket::logLevel >= level && WebSocket::logCallback) {
        char buffer[2048];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof buffer, format, args);
        va_end(args);
        logCallback(level, buffer);
    }
}
