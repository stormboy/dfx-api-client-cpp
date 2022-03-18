// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_STATUS_H
#define DFX_API_CLOUD_STATUS_H

#include "dfx/api/CloudAPI_Export.hpp"

#include <nlohmann/json.hpp>

#include <string>

typedef enum
{
    CLOUD_OK = 0,

    CLOUD_CURL_ERROR,

    CLOUD_TIMEOUT,

    // Server was not able to find the requested record
    CLOUD_RECORD_NOT_FOUND,

    CLOUD_RECORD_ALREADY_EXISTS,

    CLOUD_ALREADY_HAVE_USER_TOKEN,

    CLOUD_CONFIG_USER_TOKEN_MISSING,

    // Transport requested is not supported
    CLOUD_UNSUPPORTED_TRANSPORT,

    CLOUD_CONFIG_UNSUPPORTED_CONTENT_TYPE,
    CLOUD_CONFIG_PARSE_ERROR,

    CLOUD_TRANSPORT_FAILURE,

    CLOUD_TRANSPORT_CLOSED,

    // Operation requested is not supported by transport layer
    CLOUD_UNSUPPORTED_FEATURE,

    // Should not occur - implies missing an implementation for transport type
    CLOUD_UNIMPLEMENTED_FEATURE,

    CLOUD_PARAMETER_VALIDATION_ERROR,

    CLOUD_INTERNAL_ERROR,

    CLOUD_BAD_REQUEST,

    // User requested an operation they are not authorized to perform
    CLOUD_USER_NOT_AUTHORIZED,

    CLOUD_USER_NOT_AUTHENTICATED,

    CLOUD_TOKEN_EXPIRED,

    CLOUD_LAST // Not used
} dfx_status_code;

namespace dfx::api
{

class DFXCLOUD_EXPORT CloudStatus
{
public:
    explicit CloudStatus(dfx_status_code code);

    explicit CloudStatus(dfx_status_code code, std::string message);

    explicit CloudStatus(dfx_status_code code,
                         std::string message,
                         int protocolErrorCode,
                         std::string protocolErrorMessage);

    [[nodiscard]] bool OK() const;

    int getProtocolCode() { return protocolErrorCode; }

    dfx_status_code code;
    std::string message;
    int protocolErrorCode;
    std::string protocolErrorMessage;
};

DFXCLOUD_EXPORT std::ostream& operator<<(std::ostream& os, const CloudStatus& status);

std::string ToString(dfx_status_code code);

DFXCLOUD_EXPORT void to_json(nlohmann::json& j, const CloudStatus& s);

} // namespace dfx::api

#endif // DFX_API_CLOUD_STATUS_H
