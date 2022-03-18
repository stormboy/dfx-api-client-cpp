// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_FILTERHELPER_HPP
#define DFX_API_CLOUD_FILTERHELPER_HPP

#include "dfx/api/CloudAPI_Export.hpp"

#include <string>
#include <unordered_map>

namespace dfx::api
{
template <class T>
DFXCLOUD_EXPORT bool getFilterBool(const std::unordered_map<T, std::string>& filters,
                                   const T& searchValue,
                                   bool defaultValue)
{
    auto iter = filters.find(searchValue);
    if (iter != filters.end()) {
        auto valueStr = iter->second;
        if (valueStr.compare("true") == 0) {
            return true;
        }
    }
    return defaultValue;
}

template <class T>
DFXCLOUD_EXPORT int32_t getFilterInt32(const std::unordered_map<T, std::string>& filters,
                                       const T& searchValue,
                                       int32_t defaultValue)
{
    auto iter = filters.find(searchValue);
    if (iter != filters.end()) {
        auto valueStr = iter->second;
        size_t lastIdx(0);
        int value = std::stoi(valueStr, &lastIdx);
        if (lastIdx != valueStr.length()) {
            return defaultValue;
        }
        return value;
    }
    return defaultValue;
}

template <class T>
DFXCLOUD_EXPORT int64_t getFilterInt64(const std::unordered_map<T, std::string>& filters,
                                       const T& searchValue,
                                       int64_t defaultValue)
{
    auto iter = filters.find(searchValue);
    if (iter != filters.end()) {
        auto valueStr = iter->second;
        size_t lastIdx(0);
        int value = std::stol(valueStr, &lastIdx);
        if (lastIdx != valueStr.length()) {
            return defaultValue;
        }
        return value;
    }
    return defaultValue;
}

template <class T>
DFXCLOUD_EXPORT std::string getFilterString(const std::unordered_map<T, std::string>& filters,
                                            const T& searchValue,
                                            const std::string& defaultValue)
{
    auto iter = filters.find(searchValue);
    if (iter != filters.end()) {
        return iter->second;
    }
    return defaultValue;
}
} // namespace dfx::api

#endif // DFX_API_CLOUD_FILTERHELPER_HPP
