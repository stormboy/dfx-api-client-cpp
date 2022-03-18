// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_UTILS_HEXDUMP_H
#define DFX_API_UTILS_HEXDUMP_H

#include "dfx/api/CloudAPI_Export.hpp"
#include <cstddef>
#include <string>

namespace dfx::api::utils
{
DFXCLOUD_EXPORT std::string hexDump(const char* desc, const void* addr, size_t len);
} // namespace dfx::api::utils

#endif // DFX_API_UTILS_HEXDUMP_H
