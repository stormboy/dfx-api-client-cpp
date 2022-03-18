// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_UTILS_READFILE_H
#define DFX_API_UTILS_READFILE_H

#include "dfx/api/CloudAPI_Export.hpp"

#include <cstdint>
#include <filesystem>
#include <vector>

namespace dfx::api::utils
{
DFXCLOUD_EXPORT std::vector<uint8_t> readFile(const std::filesystem::path& filename);
DFXCLOUD_EXPORT bool writeFile(const std::filesystem::path& filename, const std::vector<uint8_t>& data);
} // namespace dfx::api::utils

#endif // DFX_API_UTILS_READFILE_H
