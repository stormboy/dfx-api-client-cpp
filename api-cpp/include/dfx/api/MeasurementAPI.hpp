// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_MEASUREMENT_API_H
#define DFX_API_CLOUD_MEASUREMENT_API_H

#include "dfx/api/CloudAPI_Export.hpp"
#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/CloudStatus.hpp"
#include "dfx/api/types/MeasurementTypes.hpp"

#include <cstdint>
#include <list>
#include <memory>
#include <string>

namespace dfx::api
{

class CloudAPI;

class DFXCLOUD_EXPORT MeasurementAPI
{
public:
    enum class MeasurementFilter
    {
        FullObject,
        StartDate,
        EndDate,
        UserProfileId,
        UserProfileName,
        StudyId,
        StatusId,
        PartnerId
    };

    /**
     * @brief MeasurementAPI constructor.
     */
    MeasurementAPI() = default;

    /**
     * @brief MeasurementAPI destructor.
     */
    virtual ~MeasurementAPI() = default;

    /**
     * \~english
     * @brief Provides a historical list of measurements captured by the API store
     *
     * The results of the measurements are captured and only displayed for the current application providers token
     * designator
     *
     * @param config provides all the cloud configuration settings
     * @param filters
     * @param measurements
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取测试记录
     *
     * 测试结果是当前用户的记录
     *
     * @param config 服务配置参数
     * @param filters
     * @param measurements	测试简略
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus list(const CloudConfig& config,
                             const std::unordered_map<MeasurementFilter, std::string>& filters,
                             uint16_t offset,
                             std::vector<Measurement>& measurements,
                             int16_t& totalCount);

    /**
     * \~english
     * @brief Returns the results of a measurement request specified by the UUID
     *
     * @param config provides all the cloud configuration settings
     * @param measurementID
     * @param measurementData
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取测试信息
     *
     * @param config 服务配置参数
     * @param measurementID	测试ID
     * @param measurementData 测试信息
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus retrieve(const CloudConfig& config,
                                 const std::string& measurementID,
                                 Measurement& measurementData);

    /**
     * \~english
     * @brief  Multiple	retrieve results of a measurements
     *
     * @param config provides all the cloud configuration settings
     * @param measurementIDs
     * @param measurements
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 批量获取测试记录
     *
     * @param config 服务配置参数
     * @param measurementIDs	测试简略
     * @param measurements 测试信息
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus retrieveMultiple(const CloudConfig& config,
                                         const std::vector<std::string>& measurementIDs,
                                         std::vector<Measurement>& measurements);
};

} // namespace dfx::api

#endif // DFX_API_CLOUD_MEASUREMENT_API_H
