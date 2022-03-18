// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#ifndef DFX_API_CLOUD_GRPC_MACROS_H
#define DFX_API_CLOUD_GRPC_MACROS_H
#else
// This header should ONLY be used from source files and is not included from headers
// So this define is only valid for the life of the compiled source module.

#error GRPC Macros should only be included from source files, not headers
#endif

#define MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(call)                                                                 \
    {                                                                                                                  \
        ::grpc::Status checkStatus = call;                                                                             \
        if (!checkStatus.ok()) {                                                                                       \
            return CloudGRPC::translateGrpcStatus(checkStatus);                                                        \
        }                                                                                                              \
    }
