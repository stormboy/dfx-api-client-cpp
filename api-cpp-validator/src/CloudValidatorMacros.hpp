// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_VALIDATOR_MACROS_H
#define DFX_API_CLOUD_VALIDATOR_MACROS_H
#else
// This header should ONLY be used from source files and is not included from headers
// So this define is only valid for the life of the compiled source module.

#error Validator Macros should only be included from source files, not headers
#endif

#include <fmt/format.h>

///////////////////////////////////////////////////////////////////////////////////////
// Macros are ugly... but so is C++ sometimes and this is nicer to ensure consistency
//
// These are localized to this file and undefined at the bottom.
///////////////////////////////////////////////////////////////////////////////////////
#define MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config)                                                                    \
    if (!config.authPassword.empty() && config.authToken.empty()) {                                                    \
        return CloudStatus(CLOUD_CONFIG_USER_TOKEN_MISSING, "Require Config.m_sUserToken, have you logged in?");       \
    }

// This one relies on string expansion and so if given:
//     MACRO_RETURN_ERROR_IF_EMPTY(config.m_sAuthUser)
// the error returned on empty will contain "config.m_sAuthUser is empty", so be careful that variable
// names make sense. It has the added benefit of string reduction as the format pattern is reused.
#define MACRO_CLOUD_API_STRINGIFY(x) #x
#define MACRO_CLOUD_API_TOSTRING(x) MACRO_CLOUD_API_STRINGIFY(x)
#define MACRO_RETURN_ERROR_IF_EMPTY(variable)                                                                          \
    if (variable.empty()) {                                                                                            \
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR,                                                           \
                           fmt::format("{} is empty", MACRO_CLOUD_API_TOSTRING(variable)));                            \
    }

#define MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(call)                                                                 \
    {                                                                                                                  \
        ::grpc::Status status = call;                                                                                  \
        if (!status.ok()) {                                                                                            \
            return CloudGRPC::translateGrpcStatus(status);                                                             \
        }                                                                                                              \
    }
