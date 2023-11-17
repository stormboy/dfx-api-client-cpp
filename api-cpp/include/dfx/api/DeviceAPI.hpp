// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_DEVICE_API_H
#define DFX_API_CLOUD_DEVICE_API_H

#include "dfx/api/CloudAPI_Export.hpp"
#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/CloudStatus.hpp"
#include "dfx/api/types/DeviceTypes.hpp"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dfx::api
{

class CloudAPI;

/**
 * \~english
 * @brief Devices are compute/mobile devices used to capture a measurement.
 *
 * Devices can be used to record the platform by which a measurement captured was
 * conducted on. The DeviceTypeID references a pre-defined set of devices with the
 * following chart. New Device Types cannot be created by organizations and are
 * managed by the API specifically. Devices types can be retrieved from a dedicated
 * endpoint returning all their values and meanings.
 *
 *\~chinese
 * @brief 设备是用于计算测量的设备数量。
 *
 * 设备可用于记录测量的数量
 * DeviceTypeID引用一组预定义的设备
 * 组织无法创建新的设备类型，并且
 * 由API专门管理。设备类型可以从专用的
 * 返回所有值和意义的端点。
 */
class DFXCLOUD_EXPORT DeviceAPI
{
public:
    /**
     * Device filters to reduce the number of values returned by list operations.
     */
    enum class DeviceFilter
    {
        FullObject,

        Date,         // REST,WebSocket: list
        EndDate,      // REST,WebSocket: list
        Name,         // REST,WebSocket: list
        Version,      // REST,WebSocket: list
        StatusID,     // REST,WebSocket: list
        DeviceTypeID, // REST,WebSocket: list
        LicenseID,    // REST,WebSocket: list
        SortOrder,    // REST,WebSocket: list

        ID // WebSocket proto: list
        // Unique,       // REST,WebSocket
    };

    /**
     * @brief DeviceAPI constructor.
     */
    DeviceAPI() = default;

    /**
     * @brief DeviceAPI destructor.
     */
    virtual ~DeviceAPI() = default;

    /**
     * \~english
     * @brief Creates a new device reference to associate with measurements. Each device
     * is mapped to a device type ID.
     *
     * @param config provides all the cloud configuration settings
     * @param name a descriptive name to associate with the device
     * @param type the device type
     * @param identifier a UUID identifier for the device
     * @param version version of the device
     * @param device the created device details
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 创建一个与测量相关的设备,
     * 每个设备都对应特定的设备类型
     *
     * @param config 服务配置参数
     * @param name 设备名称
     * @param type 设备类型
     * @param identifier 设备标识
     * @param version 设备版本
     * @param device 设备详情
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus create(const CloudConfig& config,
                               const std::string& name,
                               DeviceType type,
                               const std::string& identifier,
                               const std::string& version,
                               Device& device);

    /**
     * \~english
     * @brief Retrieves a list of existing devices in an organization.
     *
     * @param config provides all the cloud configuration settings
     * @param filters
     * @param offset to start the retrieval of records up to config.listLimit
     * @param devices list of devices being returned
     * @param totalCount the total number of records on the server
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取组织内所有设备信息
     *
     * @param config 服务配置参数
     * @param filters
     * @param offset 偏移量指定查询开始位置
     * @param devices 设备信息列表
     * @param totalCount 设备总数
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus list(const CloudConfig& config,
                             const std::unordered_map<DeviceFilter, std::string>& filters,
                             uint16_t offset,
                             std::vector<Device>& devices,
                             int16_t& totalCount);

    /**
     * \~english
     * @brief Retrieves details for a single device based on deviceID.
     *
     * @param config provides all the cloud configuration settings
     * @param deviceID the device ID to retrieve
     * @param device the retrieved device information
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 通过设备ID获取设备信息
     *
     * @param config 服务配置参数
     * @param deviceID 设备ID
     * @param device 设备信息
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus retrieve(const CloudConfig& config, const std::string& deviceID, Device& device);

    /**
     * \~english
     * @brief Retrieves details for multiple devices based on list of device IDs
     *
     * @param config provides all the cloud configuration settings
     * @param deviceIDs list of device IDs to obtain details for
     * @param devices details for the requested device IDs
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 批量获取设备信息
     *
     * @param config 服务配置参数
     * @param deviceIDs 设备ID
     * @param devices 设备信息
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus retrieveMultiple(const CloudConfig& config,
                                         const std::vector<std::string>& deviceIDs,
                                         std::vector<Device>& devices);

    /**
     * \~english
     * @brief Updates a device details.
     *
     * @param config provides all the cloud configuration settings
     * @param device details to update for the device ID
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 更新设备
     *
     * @param config 服务配置参数
     * @param device 设备信息
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus update(const CloudConfig& config, const Device& device);

    /**
     * \~english
     * @brief Removes a device from the server.
     *
     * @param config provides all the cloud configuration settings
     * @param deviceID the device ID to remove
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 删除设备
     *
     * @param config 服务配置参数
     * @param deviceID 设备ID
     * @return 接口返回值 CLOUD_OK:成功
     *
     */
    virtual CloudStatus remove(const CloudConfig& config, const std::string& deviceID);
};

} // namespace dfx::api

#endif // DFX_API_CLOUD_DEVICE_API_H
