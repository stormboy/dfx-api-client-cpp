// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/websocket/WebSocketLibWebSocket.hpp"

#include <chrono> // std::chrono::seconds
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <thread> // std::this_thread::sleep_for

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

namespace fs = std::filesystem;

// https://github.com/iamscottmoyers/simple-libwebsockets-example/blob/master/client.c

using dfx::websocket::WebSocket;
using dfx::websocket::WebSocketLibWebSocket;
using dfx::websocket::WebSocketState;

extern "C" {
static int dfx_wss_callback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len)
{
    return WebSocketLibWebSocket::dfx_wss_callback(wsi, reason, user, in, len);
}
}

int WebSocketLibWebSocket::dfx_wss_callback(
    struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len)
{
    dfx_wss_log_callback(wsi, reason, user, in, len);

    void* userdata = lws_wsi_user(wsi);

    if (userdata != nullptr) {
        auto* server = static_cast<WebSocketLibWebSocket*>(userdata);
        return server->handleEvent(wsi, reason, in, len);
    } else {
        switch (reason) {
            case LWS_CALLBACK_PROTOCOL_DESTROY:
                break;
            case LWS_CALLBACK_GET_THREAD_ID:
                break;
            case LWS_CALLBACK_EVENT_WAIT_CANCELLED:
                break;
            case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:
                break;
            case LWS_CALLBACK_PROTOCOL_INIT:
                break;
            case LWS_CALLBACK_VHOST_CERT_AGING:
                break;
            default:
                WebSocket::log(WebSocket::LOG_LEVEL_TRACE, "User data was null: UNKNOWN Event Type: %d\n", (int)reason);
                break;
        }
    }

    return 0;
}

// NOLINTNEXTLINE(modernize-avoid-c-arrays)  suggests using std::array<>
static struct lws_protocols protocols[] = {
    {
        .name                  = "proto",            /* Protocol name*/
        .callback              = ::dfx_wss_callback, /* Protocol callback */
        .per_session_data_size = 0,                  /* Protocol callback 'userdata' size */
        .rx_buffer_size        = 0,                  /* Receve buffer size (0 = no restriction) */
        .id                    = 0,                  /* Protocol Id (version) (optional) */
        .user                  = NULL,               /* 'User data' ptr, to access in 'protocol callback */
        .tx_packet_size        = 0                   /* Transmission buffer size restriction (0 = no restriction) */
    },
    {
        .name                  = "json",             /* Protocol name*/
        .callback              = ::dfx_wss_callback, /* Protocol callback */
        .per_session_data_size = 0,                  /* Protocol callback 'userdata' size */
        .rx_buffer_size        = 0,                  /* Receve buffer size (0 = no restriction) */
        .id                    = 0,                  /* Protocol Id (version) (optional) */
        .user                  = NULL,               /* 'User data' ptr, to access in 'protocol callback */
        .tx_packet_size        = 0                   /* Transmission buffer size restriction (0 = no restriction) */
    },
    LWS_PROTOCOL_LIST_TERM /* terminator */
};

WebSocketLibWebSocket::WebSocketLibWebSocket(int logLevel, LogCallback callback)
    : context(nullptr), serviceThreadJoined(true)
{
    setLogLevel(logLevel, callback);
}

WebSocketLibWebSocket::~WebSocketLibWebSocket()
{
    close();

    // If it was initialized, it is a temporary path and so we should try to delete the file on exit.
    if (!certFilePath.empty()) {
        remove(certFilePath.c_str());
    }
}

// Bridge the function signatures, libwebsocket uses a level of int and CloudLog uses uint8_t.
void WebSocketLibWebSocket::dfx_wss_log_emit(int level, const char* line)
{
    if (WebSocket::logCallback) {
        WebSocket::logCallback(static_cast<uint8_t>(level), line);
    }
}

void WebSocketLibWebSocket::setLogLevel(uint8_t level, LogCallback function)
{
    WebSocket::logLevel = level;
    WebSocket::logCallback = function;

    if (level > 0) {
        auto featureLogging = LLL_USER | LLL_HEADER | LLL_CLIENT | LLL_PARSER;
        if (level >= LOG_LEVEL_TRACE) {
            lws_set_log_level(LLL_DEBUG | LLL_INFO | LLL_NOTICE | LLL_WARN | LLL_ERR | featureLogging,
                              dfx_wss_log_emit);
        } else if (level >= LOG_LEVEL_DEBUG) {
            lws_set_log_level(LLL_INFO | LLL_NOTICE | LLL_WARN | LLL_ERR | featureLogging, dfx_wss_log_emit);
        } else if (level >= LOG_LEVEL_WARNING) {
            lws_set_log_level(LLL_WARN | LLL_ERR | featureLogging, dfx_wss_log_emit);
        } else if (level >= LOG_LEVEL_ERROR) {
            lws_set_log_level(LLL_ERR | featureLogging, dfx_wss_log_emit);
        }
    } else {
        lws_set_log_level(0, nullptr);
    }
}

void WebSocketLibWebSocket::setRootCertificate(const std::string& rootCA)
{
    // libwebsocket expects a file, not the content so write the content to temporary file

// iOS does not support the std::filesystem API, might require something else
// but there is a decent chance that configuring the rootCA on iOS is unnecessary.
#if !defined(TARGET_OS_IPHONE) || !TARGET_OS_IPHONE
    auto path = fs::temp_directory_path();
    certFilePath = path.append("cacert.pem").string();

    std::ofstream file;
    file.open(certFilePath.c_str(), std::ios::out);
    auto length = static_cast<std::streamsize>(rootCA.length());
    file.write(rootCA.c_str(), length);
    file.close();
#endif
}

void WebSocketLibWebSocket::open(const std::string& inputURL, const std::string& protocol)
{
    // Create the websockets context. This tracks open connections and knows how to route any
    // traffic and which protocol version to use, and if each connection is client or server side.
    struct lws_context_creation_info ctxCreationInfo
    {
    };
    memset(&ctxCreationInfo, 0, sizeof ctxCreationInfo);

    // Provides the data necessary to connect to another websocket server with lws_client_connect_via_info.
    struct lws_client_connect_info clientConnectInfo
    {
    };
    memset(&clientConnectInfo, 0, sizeof(clientConnectInfo));

    const char *urlProtocol = nullptr, *urlPathStart = nullptr;

    // NOLINTNEXTLINE(modernize-avoid-c-arrays)  suggests using std::array<>
    char localURLPath[1024];
    lws_strncpy(localURLPath, inputURL.data(), sizeof(localURLPath));
    if (lws_parse_uri(localURLPath, &urlProtocol, &clientConnectInfo.address, &clientConnectInfo.port, &urlPathStart)) {
        log(LOG_LEVEL_ERROR, "WebSocket: Failure to parse URI. Connection closed during setup.");
        setState(WebSocketState::CLOSED);
        return;
    }

    // Fix up the urlPath by adding a / at the beginning, copy from the parsed path start, and add a \0 at the end

    std::stringstream urlPath;
    urlPath << '/';
    if (!(urlPathStart[0] == '/' && urlPathStart[1] == '\0')) {
        urlPath << urlPathStart;
    }

    std::string urlPathString = urlPath.str();      // Temporary to protect c_str() life
    clientConnectInfo.path = urlPathString.c_str(); // Set the info's path to the fixed up url path

    const char* ca_filepath = certFilePath.c_str();

    // Set up the context creation info
    ctxCreationInfo.options = 0;                   // LWS_SERVER_OPTION_H2_JUST_FIX_WINDOW_UPDATE_OVERFLOW;
    ctxCreationInfo.port = CONTEXT_PORT_NO_LISTEN; // We don't want this client to listen
    ctxCreationInfo.protocols = protocols;         // Use our protocol list
    ctxCreationInfo.gid = -1;
    ctxCreationInfo.uid = -1;
    ctxCreationInfo.extensions = nullptr;
    ctxCreationInfo.user = this; // Callback void*

    ctxCreationInfo.ssl_ca_filepath = ca_filepath;      // info.ca_filepath;
    ctxCreationInfo.ssl_cert_filepath = nullptr;        // info.server_cert_filepath;
    ctxCreationInfo.ssl_private_key_filepath = nullptr; // info.server_private_key_filepath;
    ctxCreationInfo.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT | LWS_SERVER_OPTION_CREATE_VHOST_SSL_CTX;

    // Create the context with the info
    context = lws_create_context(&ctxCreationInfo);
    if (context == nullptr) {
        setState(WebSocketState::CLOSED);
    } else {
        // Set up the client creation info
        clientConnectInfo.context = context; // Use our created context
        if (!strcmp(urlProtocol, "https") || !strcmp(urlProtocol, "wss")) {
            clientConnectInfo.ssl_connection =
                LCCSCF_USE_SSL | LCCSCF_ALLOW_SELFSIGNED | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;
        }
        clientConnectInfo.host = clientConnectInfo.address;   // Set the connections host to the address
        clientConnectInfo.origin = clientConnectInfo.address; // Set the connections origin to the address
        clientConnectInfo.ietf_version_or_minus_one = -1;     // IETF version is -1 (the latest one)
        clientConnectInfo.userdata = this;

        if (protocol == "json") {
            clientConnectInfo.protocol = protocols[1].name;
        } else {
            clientConnectInfo.protocol = protocols[0].name;
        }

        clientConnectInfo.pwsi = &wsi; // The created client should be placed here

        // Connect with the client info
        lws_client_connect_via_info(&clientConnectInfo);
        if (wsi == nullptr) {
            setState(WebSocketState::CLOSED);
        } else {
            std::unique_lock<std::mutex> lock(shutdownMutex);
            serviceThreadJoined = false;
            serviceThread = std::make_unique<std::thread>(&WebSocketLibWebSocket::serviceThreadRunnable, this);
            setState(WebSocketState::CONNECTING);
        }
    }
}

void WebSocketLibWebSocket::serviceThreadRunnable()
{
    // Main loop runs till bExit is true, which forces an exit of this loop
    while (true) {
        auto state = getState();
        if (state == WebSocketState::CLOSING || state == WebSocketState::CLOSED) {
            break;
        }

        // LWS' function to run the message loop, which polls in this example every
        // 100 milliseconds on our created context
        const int timeout_ms = 100;
        lws_service(context, timeout_ms);
    }

    std::unique_lock<std::mutex> lock(shutdownMutex);
    setState(WebSocketState::CLOSED);
}

void WebSocketLibWebSocket::close()
{
    bool haveJoinedThread = true; // Assume it has been joined
    {
        std::unique_lock<std::mutex> lock(shutdownMutex);
        auto state = getState();
        if (state != WebSocketState::CLOSED) {
            if (state != WebSocketState::CLOSING) {
                setState(WebSocketState::CLOSING);
            }
        }
        haveJoinedThread = serviceThreadJoined;
    }

    if (!haveJoinedThread) {
        serviceThread->join();

        std::unique_lock<std::mutex> lock(shutdownMutex);
        lws_context_destroy(context); // Can not call while lws_service() in play
        serviceThreadJoined = true;
        setState(WebSocketState::CLOSED);
    } else {
        // Nothing to do - service thread already joined, and so socket is closed
    }
}

void WebSocketLibWebSocket::sendUTF8(const std::string& str)
{
    auto buffer = std::make_shared<std::vector<uint8_t>>();
    buffer->resize(LWS_PRE, 0);
    std::copy(str.begin(), str.end(), std::back_inserter(*buffer));

    sendFormattedBinaryBuffer(buffer);
}

void WebSocketLibWebSocket::sendBinary(const std::vector<uint8_t>& data)
{
    auto buffer = std::make_shared<std::vector<uint8_t>>();
    buffer->resize(LWS_PRE, 0);
    std::copy(data.begin(), data.end(), std::back_inserter(*buffer));

    sendFormattedBinaryBuffer(buffer);
}

void WebSocketLibWebSocket::sendFormattedBinaryBuffer(std::shared_ptr<std::vector<uint8_t>> buffer)
{
    {
        std::unique_lock<std::mutex> lock(mutex);

        // This is client/server thread... we should not send unless we can do it without blocking.
        pendingSendData.push_back(std::move(buffer));
    }

    if (getState() == WebSocketState::OPEN) {
        // Add it to the queue of things to send, and check to see if it would block. The event that comes
        // back will have our background thread send the event. If we are open, send a callback request. If
        // we are not open yet, the open will handle it.
        lws_callback_on_writable(wsi);
    }
}

int WebSocketLibWebSocket::handleEvent(struct lws* wsi, enum lws_callback_reasons reason, void* in, size_t len)
{
    bool shouldLog = false;
    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED: {
            setState(WebSocketState::OPEN);

            // We have an established socket, but might still block if we were to write. To avoid the
            // client blocking, ask to get notified when socket is good to send something.
            lws_callback_on_writable(wsi);

            WebSocketEvent event;
            event.type = WebSocketEventType::OPEN;
            notifyClient(event);
            break;
        }
        case LWS_CALLBACK_WS_CLIENT_DROP_PROTOCOL: {
            // Ignore, nothing to do, the next event will be a LWS_CALLBACK_CLIENT_CLOSED
            // which performs cleanup.
            break;
        }
        case LWS_CALLBACK_CLIENT_CLOSED: {
            setState(WebSocketState::CLOSING);

            WebSocketEvent event;
            event.type = WebSocketEventType::CLOSED;

            notifyClient(event);

            return -1; // We want it closed
        }
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR: {
            setState(WebSocketState::CLOSING); // Assume errors are non-recoverable

            std::string message(static_cast<char*>(in));

            WebSocketErrorEvent error;
            error.code = reason;
            error.message = message;

            WebSocketEvent event;
            event.type = WebSocketEventType::ERROR_EVENT;
            event.error = error;

            notifyClient(event);
            break;
        }
        case LWS_CALLBACK_CLIENT_RECEIVE: {
            // To support fragmented messages would need to check for the final frame of a message
            // with lws_is_final_fragment. This check can be combined with
            // libwebsockets_remaining_packet_payload to gather the whole contents of a message.

            WebSocketEvent event;
            event.type = WebSocketEventType::MESSAGE;
            event.message.data =
                std::make_shared<std::vector<uint8_t>>(static_cast<uint8_t*>(in), (static_cast<uint8_t*>(in) + len));
            event.message.isText = false;

            notifyClient(event);
            break;
        }
        case LWS_CALLBACK_CLIENT_WRITEABLE: {
            std::unique_lock<std::mutex> lock(mutex); // Protect - pendingSendData

            // We are writable... send what might be queued up.
            if (!pendingSendData.empty()) {
                auto data = pendingSendData.front();
                pendingSendData.pop_front();

                lws_write_protocol protocol = LWS_WRITE_BINARY;
                lws_write(wsi, data->data() + LWS_PRE, data->size() - LWS_PRE, protocol);

                // If still something to send in the queue, check for when it is safe to send without blocking
                if (!pendingSendData.empty()) {
                    lws_callback_on_writable(wsi);
                }
            }
            break;
        }
        case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:
            break;
        case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER: {
            // We can ignore - the new API allows us to upgrade without injecting header tokens
            break;
        }
        default:
            break;
    }

    return 0;
}

void WebSocketLibWebSocket::dfx_wss_log_callback(
    struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len)
{
#ifndef NDEBUG // Only include this when in DEBUG mode
    if (WebSocket::logLevel >= LOG_LEVEL_TRACE) {
        switch (reason) {
            case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:
                WebSocket::log(LOG_LEVEL_TRACE,
                               "LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS: %d\n",
                               LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS);
                break;
            case LWS_CALLBACK_PROTOCOL_INIT:
                WebSocket::log(LOG_LEVEL_TRACE, "LWS_CALLBACK_PROTOCOL_INIT: %d\n", LWS_CALLBACK_PROTOCOL_INIT);
                break;
            case LWS_CALLBACK_CLIENT_HTTP_BIND_PROTOCOL:
                WebSocket::log(LOG_LEVEL_TRACE,
                               "LWS_CALLBACK_CLIENT_HTTP_BIND_PROTOCOL: %d\n",
                               LWS_CALLBACK_CLIENT_HTTP_BIND_PROTOCOL);
                break;
            case LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED:
                WebSocket::log(LOG_LEVEL_TRACE,
                               "LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED: %d\n",
                               LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED);
                break;
            case LWS_CALLBACK_GET_THREAD_ID:
                WebSocket::log(LOG_LEVEL_TRACE, "LWS_CALLBACK_GET_THREAD_ID: %d\n", LWS_CALLBACK_GET_THREAD_ID);
                break;
            case LWS_CALLBACK_EVENT_WAIT_CANCELLED:
                WebSocket::log(
                    LOG_LEVEL_TRACE, "LWS_CALLBACK_EVENT_WAIT_CANCELLED: %d\n", LWS_CALLBACK_EVENT_WAIT_CANCELLED);
                break;
            case LWS_CALLBACK_WSI_CREATE:
                WebSocket::log(LOG_LEVEL_TRACE, "LWS_CALLBACK_WSI_CREATE: %d\n", LWS_CALLBACK_WSI_CREATE);
                break;
            case LWS_CALLBACK_WS_PEER_INITIATED_CLOSE: // think this happens if we don't respond when we should
                WebSocket::log(LOG_LEVEL_TRACE,
                               "LWS_CALLBACK_WS_PEER_INITIATED_CLOSE: %d\n",
                               LWS_CALLBACK_WS_PEER_INITIATED_CLOSE);
                break;
            case LWS_CALLBACK_OPENSSL_PERFORM_SERVER_CERT_VERIFICATION:
                WebSocket::log(LOG_LEVEL_TRACE,
                               "LWS_CALLBACK_OPENSSL_PERFORM_SERVER_CERT_VERIFICATION: %d\n",
                               LWS_CALLBACK_OPENSSL_PERFORM_SERVER_CERT_VERIFICATION);
                break;
            case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
                WebSocket::log(LOG_LEVEL_TRACE,
                               "LWS_CALLBACK_CLIENT_CONNECTION_ERROR: %d\n",
                               LWS_CALLBACK_CLIENT_CONNECTION_ERROR);
                break;
            case LWS_CALLBACK_WSI_DESTROY:
                WebSocket::log(LOG_LEVEL_TRACE, "LWS_CALLBACK_WSI_DESTROY: %d\n", LWS_CALLBACK_WSI_DESTROY);
                break;
            case LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP:
                WebSocket::log(LOG_LEVEL_TRACE,
                               "LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP: %d\n",
                               LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP);
                break;
            case LWS_CALLBACK_CLIENT_ESTABLISHED:
                WebSocket::log(
                    LOG_LEVEL_TRACE, "LWS_CALLBACK_CLIENT_ESTABLISHED: %d\n", LWS_CALLBACK_CLIENT_ESTABLISHED);
                break;
            case LWS_CALLBACK_CLIENT_WRITEABLE:
                WebSocket::log(LOG_LEVEL_TRACE, "LWS_CALLBACK_CLIENT_WRITEABLE: %d\n", LWS_CALLBACK_CLIENT_WRITEABLE);
                break;
            case LWS_CALLBACK_CLIENT_RECEIVE:
                WebSocket::log(LOG_LEVEL_TRACE, "LWS_CALLBACK_CLIENT_RECEIVE: %d\n", LWS_CALLBACK_CLIENT_RECEIVE);
                break;
            case LWS_CALLBACK_CLIENT_RECEIVE_PONG:
                WebSocket::log(
                    LOG_LEVEL_TRACE, "LWS_CALLBACK_CLIENT_RECEIVE_PONG: %d\n", LWS_CALLBACK_CLIENT_RECEIVE_PONG);
                break;
            case LWS_CALLBACK_WS_CLIENT_DROP_PROTOCOL:
                WebSocket::log(LOG_LEVEL_TRACE,
                               "LWS_CALLBACK_WS_CLIENT_DROP_PROTOCOL: %d\n",
                               LWS_CALLBACK_WS_CLIENT_DROP_PROTOCOL);
                break;
            case LWS_CALLBACK_CLIENT_CLOSED:
                WebSocket::log(LOG_LEVEL_TRACE, "LWS_CALLBACK_CLIENT_CLOSED: %d\n", LWS_CALLBACK_CLIENT_CLOSED);
                break;
            case LWS_CALLBACK_CLIENT_HTTP_DROP_PROTOCOL:
                WebSocket::log(LOG_LEVEL_TRACE,
                               "LWS_CALLBACK_CLIENT_HTTP_DROP_PROTOCOL: %d\n",
                               LWS_CALLBACK_CLIENT_HTTP_DROP_PROTOCOL);
                break;
            case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
                WebSocket::log(LOG_LEVEL_TRACE,
                               "LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER: %d\n",
                               LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER);
                break;
            case LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH:
                WebSocket::log(LOG_LEVEL_TRACE,
                               "LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH: %d\n",
                               LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH);
                break;
            case LWS_CALLBACK_PROTOCOL_DESTROY:
                WebSocket::log(LOG_LEVEL_TRACE, "LWS_CALLBACK_PROTOCOL_DESTROY: %d\n", LWS_CALLBACK_PROTOCOL_DESTROY);
                break;
            case LWS_CALLBACK_CONNECTING:
                WebSocket::log(LOG_LEVEL_TRACE, "LWS_CALLBACK_CONNECTING: %d\n", LWS_CALLBACK_CONNECTING);
                break;
            case LWS_CALLBACK_VHOST_CERT_AGING:
                WebSocket::log( LOG_LEVEL_TRACE, "LWS_CALLBACK_VHOST_CERT_AGING: %d\n", LWS_CALLBACK_VHOST_CERT_AGING);
                break;
            default:
                WebSocket::log(LOG_LEVEL_TRACE, "callback UNKNOWN: %d\n", reason);
        }
    }
#endif
}
