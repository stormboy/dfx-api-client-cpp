// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_SIGNAL_API_H
#define DFX_API_CLOUD_SIGNAL_API_H

#include "dfx/api/CloudAPI_Export.hpp"
#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/CloudStatus.hpp"
#include "dfx/api/types/SignalTypes.hpp"

#include <list>
#include <memory>
#include <string>

namespace dfx::api
{

class CloudAPI;

class DFXCLOUD_EXPORT SignalAPI
{
public:
    enum class SignalFilter
    {
        FullObject
    };

    /**
     * @brief SignalAPI constructor.
     */
    SignalAPI() = default;

    /**
     * @brief SignalAPI destructor.
     */
    virtual ~SignalAPI() = default;

    /**
     * \~english
     * @brief Lists the known signals
     *
     * @param config provides all the cloud configuration settings
     * @param filters
     * @param offset to start the retrieval of records up to config.listLimit
     * @param signals
     * @param totalCount the total number of records on the server
     * @return status of operation, CLOUD_OK on SUCCESS
     */
    virtual CloudStatus list(const CloudConfig& config,
                             const std::unordered_map<SignalFilter, std::string>& filters,
                             uint16_t offset,
                             std::vector<Signal>& signals,
                             int16_t& totalCount);

    /**
     * \~english
     * @brief Retrieves a single Signal specified by ID. If the
     * record is found the other fields of the signal instance will be returned.
     *
     * @param config provides all the cloud configuration settings
     * @param signalID
     * @param signal
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     *
     * @param config 服务配置参数
     * @param signalID
     * @param signal
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus retrieve(const CloudConfig& config, const std::string& signalID, Signal& signal);

    /**
     * \~english
     * @brief Retrieves details for multiple signals based on list of signal IDs
     *
     * @param config provides all the cloud configuration settings
     * @param signalIDs list of signal IDs to obtain details for
     * @param signals details for the requested signal IDs
     * @return status of operation, CLOUD_OK on SUCCESS
     */
    virtual CloudStatus retrieveMultiple(const CloudConfig& config,
                                         const std::vector<std::string>& signalIDs,
                                         std::vector<Signal>& signals);

    /**
     * \~english
     * @brief retrieve signalIDs in study
     *
     * @param config provides all the cloud configuration settings
     * @param studyID
     * @param signalIDs
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取研究的信号
     *
     * @param config 服务配置参数
     * @param studyID 研究ID
     * @param signalIDs	信号ID
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus retrieveStudySignalIDs(const CloudConfig& config,
                                               const std::string& studyID,
                                               std::vector<std::string>& signalIDs);

    /**
     * \~english
     * @brief retrieve signal detail
     *
     * @param config provides all the cloud configuration settings
     * @param signalID
     * @param signalDetail
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取信号详情
     *
     * @param config 服务配置参数
     * @param signalID 信号ID
     * @param signalDetail 信号详情
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus retrieveSignalDetail(const CloudConfig& config,
                                             const std::string& signalID,
                                             Signal& signalDetail);

    /**
     * \~english
     * @brief Retrieves details for multiple signals
     *
     * @param config provides all the cloud configuration settings
     * @param signalIDs
     * @param signalDetails
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 批量获取信号信息
     *
     * @param config 服务配置参数
     * @param signalIDs	信号ID
     * @param signalDetails	信号详情
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus retrieveSignalDetails(const CloudConfig& config,
                                              const std::list<std::string>& signalIDs,
                                              std::vector<Signal>& signalDetails);
};

} // namespace dfx::api

#endif // DFX_API_CLOUD_SIGNAL_API_H
