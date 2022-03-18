// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFXAPI_WEBSOCKET_HPP
#define DFXAPI_WEBSOCKET_HPP

#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace dfx::websocket
{

class WebSocket;

enum class WebSocketState
{
    CREATED,    // Socket has not yet been requested to open.
    CONNECTING, // Socket has been created. The connection is not yet open.
    OPEN,       // The connection is open and ready to communicate.
    CLOSING,    // The connection is in the process of closing.
    CLOSED      // The connection is closed or couldn't be opened.
};

enum class WebSocketEventType
{
    ERROR_EVENT, // Error encountered, event contains WebSocketErrorEvent.
    OPEN,        // Open event notification.
    LISTEN,      // Listen event notification (for servers).
    CONNECTION,  // Connection event notification (for servers).
    MESSAGE,     // Message received, event contains WebSocketMessageEvent.

    CLOSED
};

// Status of the method call.
enum class Status
{
    OK,
    FAILURE,
    TIMEOUT
};

struct WebSocketErrorEvent
{
    int code = 0;
    std::string message;
};

struct WebSocketMessageEvent
{
    std::shared_ptr<std::vector<uint8_t>> data;
    bool isText = false;
};

struct WebSocketEvent
{
    WebSocketEventType type;

    // Technically a union but can't place non-primitive types in a union, the memory overhead
    // of this is a non-issue so declaring both, check type to know which is active while the
    // other will be a default object.
    WebSocketErrorEvent error;
    WebSocketMessageEvent message;
};

typedef std::function<void(const WebSocketEvent&, void*)> WebSocketEventCallback;

typedef std::function<void(uint8_t, const char*)> LogCallback;

class WebSocket : public std::enable_shared_from_this<WebSocket>
{
public:
    static const uint8_t LOG_LEVEL_ERROR = 1;
    static const uint8_t LOG_LEVEL_WARNING = 2;
    static const uint8_t LOG_LEVEL_DEBUG = 3;
    static const uint8_t LOG_LEVEL_TRACE = 4;

    /**
     * Constructs a WebSocket which will be backed by either a libwebsocket or a Emscripten
     * WebSocket depending upon the compilation target.
     */
    static std::shared_ptr<WebSocket> create(int logLevel, LogCallback callback);

    virtual ~WebSocket() = default;

    /**
     * Asynchronously get notified of events as they are received by setting up a callback.
     */
    void setEventCallback(const WebSocketEventCallback& callback, void* userData);

    /**
     * Synchronously poll events, waiting for timeout. 0 is indefinite.
     */
    Status getEvent(WebSocketEvent& event, uint32_t timeout);

    void setState(WebSocketState state);

    WebSocketState getState();

    /**
     * SSL requires
     */
    virtual void setRootCertificate(const std::string& rootCA) = 0;

    virtual void setLogLevel(uint8_t level, LogCallback function) = 0;

    virtual void open(const std::string& uri, const std::string& protocol) = 0;

    virtual void close() = 0;

    virtual void sendUTF8(const std::string& data) = 0;

    // Sends the given block of raw memory data out to the connected server.
    virtual void sendBinary(const std::vector<uint8_t>& data) = 0;

protected:
    WebSocket();

    /**
     * Helper for the implementation to pass received events from the server back to
     * the client. If the client has initialized with setEventCallback, it will be
     * notified of the event or the event will be queued for the client to poll with
     * getEvent.
     */
    void notifyClient(const WebSocketEvent& event);

    // These are unfortunately static members because I want to use them in callbacks which
    // may not have a direct reference to this instance. Given the unlikely scenario of logging
    // with multiple instances this seems like a reasonable trade-off.
    static uint8_t logLevel;
    static LogCallback logCallback;

    static void log(uint8_t level, const char* format, ...);

private:
    std::mutex mutex;

    /**
     * The state of the underlying WebSocket.
     * @see WebSocketState
     */
    WebSocketState state;

    WebSocketEventCallback onEventCallback;
    void* eventUserData;

    std::mutex pendingEventsMutex;
    std::deque<WebSocketEvent> pendingEvents;
};

} // namespace dfx::websocket

#endif // DFXAPI_WEBSOCKET_HPP
