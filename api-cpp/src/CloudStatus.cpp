// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/CloudStatus.hpp"

#include <utility>

using namespace dfx::api;

CloudStatus::CloudStatus(dfx_status_code code) : CloudStatus(code, ""){};

CloudStatus::CloudStatus(dfx_status_code code, std::string message)
    : code(code), message(std::move(message)), protocolErrorCode(0), protocolErrorMessage(""){};

CloudStatus::CloudStatus(dfx_status_code code,
                         std::string message,
                         int protocolErrorCode,
                         std::string protocolErrorMessage)
    : code(code), message(std::move(message)), protocolErrorCode(protocolErrorCode),
      protocolErrorMessage(std::move(protocolErrorMessage)){

      };

bool CloudStatus::OK() const
{
    return code == CLOUD_OK;
}

std::ostream& dfx::api::operator<<(std::ostream& os, const CloudStatus& status)
{
    os << ToString(status.code) << ": " << status.message;
    return os;
}

std::string dfx::api::ToString(dfx_status_code code)
{
    switch (code) {
        case CLOUD_OK:
            return "CLOUD_OK";
        case CLOUD_TIMEOUT:
            return "CLOUD_TIMEOUT";
        case CLOUD_RECORD_NOT_FOUND:
            return "CLOUD_RECORD_NOT_FOUND";
        case CLOUD_RECORD_ALREADY_EXISTS:
            return "CLOUD_RECORD_ALREADY_EXISTS";
        case CLOUD_UNSUPPORTED_TRANSPORT:
            return "CLOUD_UNSUPPORTED_TRANSPORT";
        case CLOUD_BAD_REQUEST:
            return "CLOUD_BAD_REQUEST";
        case CLOUD_PARAMETER_VALIDATION_ERROR:
            return "CLOUD_PARAMETER_VALIDATION_ERROR";
        case CLOUD_INTERNAL_ERROR:
            return "CLOUD_INTERNAL_ERROR";
        case CLOUD_UNSUPPORTED_FEATURE:
            return "CLOUD_UNSUPPORTED_FEATURE";
        case CLOUD_UNIMPLEMENTED_FEATURE:
            return "CLOUD_UNIMPLEMENTED_FEATURE";
        case CLOUD_TRANSPORT_FAILURE:
            return "CLOUD_TRANSPORT_FAILURE";
        case CLOUD_TRANSPORT_CLOSED:
            return "CLOUD_TRANSPORT_CLOSED";
        case CLOUD_CONFIG_UNSUPPORTED_CONTENT_TYPE:
            return "CLOUD_CONFIG_UNSUPPORTED_CONTENT_TYPE";
        case CLOUD_USER_NOT_AUTHENTICATED:
            return "CLOUD_USER_NOT_AUTHENTICATED";
        case CLOUD_USER_NOT_AUTHORIZED:
            return "CLOUD_USER_NOT_AUTHORIZED";
        case CLOUD_TOKEN_EXPIRED:
            return "CLOUD_TOKEN_EXPIRED";
        default:
            return "CloudStatus(" + std::to_string(code) + ")";
    }
}

void dfx::api::to_json(nlohmann::json& j, const CloudStatus& s)
{
    j = nlohmann::json{{"Message", s.message}, {"Code", ToString(s.code)}};
    if (s.protocolErrorCode != 0 || !s.protocolErrorMessage.empty()) {
        j["ProtocolCode"] = s.protocolErrorCode;
        j["ProtocolError"] = s.protocolErrorMessage;
    }
}
