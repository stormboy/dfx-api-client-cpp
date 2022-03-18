// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/CloudLog.hpp"

#include <cstdarg>
#include <iostream>
#include <stdlib.h>

#ifdef WITH_GRPC
// Don't really know if the gRPC endpoint will be used, but want logs if config verbose level enabled
// Valid GRPC_VERBOSITY levels are: DEBUG, INFO and ERROR
const std::string GRPC_VERBOSITY("GRPC_VERBOSITY");
const std::string GRPC_DEBUG_LEVEL("DEBUG");
const std::string GRPC_INFO_LEVEL("INFO");
const std::string GRPC_ERROR_LEVEL("ERROR");
const std::string GRPC_TRACE("TRACE");
const std::string GRPC_ALL("all");
#endif

static bool loggingEnabled = false;
static int loggingLevel = 0;

bool dfx::api::cloudLogEnabled()
{
    return loggingEnabled;
}

void dfx::api::cloudLogSetEnabled(bool enabled)
{
    if (loggingEnabled != enabled) {
        loggingEnabled = enabled;
    }
}

bool dfx::api::cloudLogIsActive(int logLevel)
{
    return true;
}

int dfx::api::cloudLogLevel()
{
    return loggingLevel;
}

void dfx::api::cloudLogSetLevel(int logLevel)
{
    loggingLevel = logLevel;

#ifdef WITH_GRPC
    switch (logLevel) {
        case CLOUD_LOG_LEVEL_NONE:
            unsetenv(GRPC_VERBOSITY.c_str());
            unsetenv(GRPC_TRACE.c_str());
            break;
        case CLOUD_LOG_LEVEL_ERROR:
        case CLOUD_LOG_LEVEL_WARNING:
            setenv(GRPC_VERBOSITY.c_str(), GRPC_ERROR_LEVEL.c_str(), 1);
            break;
        case CLOUD_LOG_LEVEL_INFO:
            setenv(GRPC_VERBOSITY.c_str(), GRPC_INFO_LEVEL.c_str(), 1);
            break;
        case CLOUD_LOG_LEVEL_DEBUG:
        case CLOUD_LOG_LEVEL_TRACE:
            setenv(GRPC_VERBOSITY.c_str(), GRPC_DEBUG_LEVEL.c_str(), 1);
            break;
    }
    setenv(GRPC_TRACE.c_str(), GRPC_ALL.c_str(), 0);
#endif
}

void dfx::api::cloudLog(uint8_t level, const char* format, ...)
{
    if (cloudLogLevel() >= level) {
        char buffer[2048];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof buffer, format, args);
        va_end(args);

        std::string levelString("UNKNOWN");
        switch (level) {
            case CLOUD_LOG_LEVEL_ERROR:
                levelString = "ERROR";
                break;
            case CLOUD_LOG_LEVEL_WARNING:
                levelString = "WARNING";
                break;
            case CLOUD_LOG_LEVEL_INFO:
                levelString = "INFO";
                break;
            case CLOUD_LOG_LEVEL_DEBUG:
                levelString = "DEBUG";
                break;
            case CLOUD_LOG_LEVEL_TRACE:
                levelString = "TRACE";
                break;
        }
        std::cerr << levelString.c_str() << ": " << buffer << std::flush;
    }
}
