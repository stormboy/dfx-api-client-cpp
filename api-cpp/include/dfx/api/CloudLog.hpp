// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_LOG_H
#define DFX_API_CLOUD_LOG_H

#include "dfx/api/CloudAPI_Export.hpp"

#include <string>

#include "CloudTypes.hpp"

namespace dfx::api
{
const uint8_t CLOUD_LOG_LEVEL_NONE = 0;
const uint8_t CLOUD_LOG_LEVEL_ERROR = 1;
const uint8_t CLOUD_LOG_LEVEL_WARNING = 2;
const uint8_t CLOUD_LOG_LEVEL_INFO = 3;
const uint8_t CLOUD_LOG_LEVEL_DEBUG = 4;
const uint8_t CLOUD_LOG_LEVEL_TRACE = 5;

DFXCLOUD_EXPORT bool cloudLogEnabled();

DFXCLOUD_EXPORT void cloudLogSetEnabled(bool enabled);

DFXCLOUD_EXPORT bool cloudLogIsActive(int logLevel);

DFXCLOUD_EXPORT int cloudLogLevel();

DFXCLOUD_EXPORT void cloudLogSetLevel(int logLevel);

DFXCLOUD_EXPORT void cloudLog(uint8_t level, const char* format, ...);

} // namespace dfx::api

#endif // DFX_API_CLOUD_LOG_H
