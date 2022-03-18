// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_LICENSE_API_H
#define DFX_API_CLOUD_LICENSE_API_H

#include "dfx/api/CloudAPI_Export.hpp"
#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/CloudStatus.hpp"
#include "dfx/api/types/LicenseTypes.hpp"

#include <cstdint>
#include <list>
#include <memory>
#include <string>

namespace dfx::api
{
class CloudAPI;

/**
 * \~english
 * @brief Licenses enable organizations to activate devices and create measurements.
 *
 * \~chinese
 * @brief 许可证使组织能够激活设备和创建测量.
 */
class DFXCLOUD_EXPORT LicenseAPI
{
public:
    enum class LicenseFilter
    {
        FullObject
    };

    /**
     * @brief LicenseAPI constructor.
     */
    LicenseAPI() = default;

    /**
     * @brief LicenseAPI destructor.
     */
    virtual ~LicenseAPI() = default;

    /**
     * \~english
     * @brief Lists Licenses available to your User / Organization.
     *
     * @param config provides all the cloud configuration settings
     * @param offset to start the retrieval of records up to config.listLimit
     * @param licenses
     * @param totalCount the total number of records on the server
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 列出所有许可证(当前用户或者组织相关的)
     *
     * @param config 服务配置参数
     * @param offset 偏移量指定查询开始位置
     * @param licenses 许可证列表
     * @param totalCount 许可证总数
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus list(const CloudConfig& config,
                             const std::unordered_map<LicenseFilter, std::string>& filters,
                             uint16_t offset,
                             std::vector<License>& licenses,
                             int16_t& totalCount);
};

} // namespace dfx::api

#endif // DFX_API_CLOUD_LICENSE_API_H
