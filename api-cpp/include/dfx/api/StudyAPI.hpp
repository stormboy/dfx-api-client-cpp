// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_STUDY_API_H
#define DFX_API_CLOUD_STUDY_API_H

#include "dfx/api/CloudAPI_Export.hpp"
#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/CloudStatus.hpp"
#include "dfx/api/types/StudyTypes.hpp"

#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <string>

namespace dfx::api
{

class CloudAPI;

class DFXCLOUD_EXPORT StudyAPI
{
public:
    enum class StudyFilter
    {
        FullObject
    };

    /**
     * @brief StudyAPI constructor.
     */
    StudyAPI() = default;

    /**
     * @brief StudyAPI destructor.
     */
    virtual ~StudyAPI() = default;

    /**
     * \~english
     * @brief Create study will construct a new study based on the studyTemplateID.
     *
     * Studies *must* be based on a pre-existing studyTemplateID.
     *
     * Passing in config will override values available in the StudyType definition template.
     *
     * \verbatim embed:rst:leading-asterisk
     *     .. warning::
     *
     *        The gRPC backend currently *ignores* any values passed in studyConfig map.
     *
     * \endverbatim
     *
     * @param config provides all the cloud configuration settings
     * @param name the name to give the newly created study
     * @param description a short explanation for why this new study is needed
     * @param studyTemplateID the template to base this new study off
     * @param studyConfig any properties in the existing study to override
     * @param studyID the newly created study identifier returned on CLOUD_OK
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 创建研究
     *
     * 必须根据研究模板创建
     *
     * @param config 服务配置参数
     * @param name 名称
     * @param description 简述
     * @param studyTemplateID 研究模板ID
     * @param studyConfig 研究配置信息
     * @param studyID 生成的研究ID
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus create(const CloudConfig& config,
                               const std::string& name,
                               const std::string& description,
                               const std::string& studyTemplateID,
                               const std::map<std::string, std::string>& studyConfig,
                               std::string& studyID);

    /**
     * \~english
     * @brief Retrieve a summary list of the studies.
     *
     * @param config provides all the cloud configuration settings
     * @param filters
     * @param offset to start the retrieval of records up to config.listLimit
     * @param studies the returned studies
     * @param totalCount the total number of records on the server
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取研究简述列表
     *
     * @param config 服务配置参数
     * @param filters
     * @param offset 偏移量指定查询开始位置
     * @param studies 研究简述列表
     * @param totalCount 配置文件总数
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus list(const CloudConfig& config,
                             const std::unordered_map<StudyFilter, std::string>& filters,
                             uint16_t offset,
                             std::vector<Study>& studies,
                             int16_t& totalCount);

    /**
     * \~english
     * @brief Retrieve full study details for a specific studyID.
     *
     * @param config provides all the cloud configuration settings
     * @param studyID the study to retrieve the details
     * @param study the returned study details on CLOUD_OK
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取研究详情
     *
     * @param config 服务配置参数
     * @param studyID 研究ID
     * @param study 研究详情
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus retrieve(const CloudConfig& config, const std::string& studyID, Study& study);

    /**
     * \~english
     * @brief Retrieve full study details for a StudySummary list.
     *
     * Typically, this will be used in conjunction with the ``list`` method
     * when more details are required beyond just the StudySummary id and name.
     *
     * @param config provides all the cloud configuration settings
     * @param studyIDs contains the list of study IDs
     * @param studies the returned list of studies on CLOUD_OK
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 批量获取研究信息
     *
     * @param config 服务配置参数
     * @param studyIDs 研究ID列表
     * @param studies 研究信息
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus retrieveMultiple(const CloudConfig& config,
                                         const std::vector<std::string>& studyIDs,
                                         std::vector<Study>& studies);

    /**
     * \~english
     * @breif Update an existing study based on the provided studyID.
     *
     * Updates a particular study record with new information. Organizations can set the status of a
     * particular study as well to record their name and description.
     *
     * @param config provides all the cloud configuration settings
     * @param studyID the study identifier to update
     * @param name the new name for the study
     * @param description the new description to use for the study
     * @param studyStatus the new status for the study
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @breif 更新研究
     *
     * @param config 服务配置参数
     * @param studyID 研究ID
     * @param name 名称
     * @param description 简述
     * @param studyStatus 状态
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus update(const CloudConfig& config,
                               const std::string& studyID,
                               const std::string& name,
                               const std::string& description,
                               StudyStatus status);

    /**
     * \~english
     * @brief Remove an existing study based on the provided studyID.
     *
     * \verbatim embed:rst:leading-asterisk
     *     .. note::
     *
     *        For studies which have data retention, if there are any measurements associated
     *        with the study it can not be removed.
     *
     * \endverbatim
     *
     * @param config provides all the cloud configuration settings
     * @param studyID the study identifier to remove
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 删除研究
     *
     * 如果有相关测试进行,则不能删除
     *
     * @param config 服务配置参数
     * @param studyID 研究ID
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus remove(const CloudConfig& config, const std::string& studyID);

    /**
     * \~english
     * @brief Retrieve the SDK Study Config data.
     *
     * The SDK needs to be populated with a study config so that it is able to provide the server
     * with the correct data for the measurement. The server is configurable and different studies
     * have different pieces of data that it requires as inputs to the calculation engine. The
     * returned binary data is used by the SDK to configure the payloads to contain the expected
     * data.
     *
     * The call can be passed the current hashID if it exists from a previous retrieve which
     * will enable the server to determine if the current version is the latest in which case
     * it will not need to send back an update from the server in which case the response will
     * be CLOUD_OK and the studyData will be left untouched but the hashID will be updated to
     * match the currentMD5Hash.
     *
     * If the call is passed an empty string for the current hashID it will update both the
     * studyData and hashID appropriately, assuming the study data exists on the server.
     *
     * \verbatim embed:rst:leading-asterisk
     *     .. code:: cpp
     *
     *        std::vector<uint8_t> studyData;
     *        std::string hashID;
     *        auto sdkID = dfxFactory->getSDKID();
     *
     *        auto status = cloudAPI->retrieveStudyConfig(config, config.studyID,
     *                                 sdkID, hashID, studyData, hashID);
     *
     *        if ( !status.OK() ) {
     *              // Something went wrong - handle with error
     *              std::cerr << "Unable to retrieve study config" << std::endl;
     *        } else {
     *              if ( !dfxFactory->initializeStudy(studyData) ) {
     *                 std::cerr << "Unable to initialize the study" << std::endl;
     *              } else {
     *                 auto collector = dfxFactory->createCollector();
     *
     *                 // Perform collection...
     *
     *              }
     *        }
     *
     * \endverbatim
     *
     * @param config provides all the cloud configuration settings
     * @param studyID the study identifier to retrieve config data for
     * @param sdkID the SDK version identifier, so the server knows what client can handle
     * @param currentHashID hashID returned from previous call, or empty string if have no data yet
     * @param studyData the returned study data for the requested studyID and sdkID
     * @param hashID the hashID for the returned studyData
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取SDK研究配置
     *
     * SDK需要填充一个研究配置，以便它能够提供服务器
     * 具有正确的测量数据。服务器是可配置的和不同的研究
     * 有不同的数据段作为计算引擎的输入。
     * SDK使用返回的二进制数据来配置有效负载以包含预期的数据。
     *
     * 如果当前hashID存在于先前的检索中，则可以将其传递给调用
     * 将使服务器能够确定当前版本是否为最新版本
     * 它将不需要从服务器发回更新。studyData将保持不变，但hashID将更新为
     * 匹配currentMD5Hash。
     *
     * 如果调用被传递给当前hashID的空字符串，它将更新
     * 假设服务器上存在研究数据，则studyData和hashID是适当的。
     *
     * \verbatim embed:rst:leading-asterisk
     *     .. code:: cpp
     *
     *        std::vector<uint8_t> studyData;
     *        std::string hashID;
     *        auto sdkID = dfxFactory->getSDKID();
     *
     *        auto status = cloudAPI->retrieveStudyConfig(config, config.studyID,
     *                                 sdkID, hashID, studyData, hashID);
     *
     *        if ( !status.OK() ) {
     *              // Something went wrong - handle with error
     *              std::cerr << "Unable to retrieve study config" << std::endl;
     *        } else {
     *              if ( !dfxFactory->initializeStudy(studyData) ) {
     *                 std::cerr << "Unable to initialize the study" << std::endl;
     *              } else {
     *                 auto collector = dfxFactory->createCollector();
     *
     *                 // Perform collection...
     *
     *              }
     *        }
     *
     * \endverbatim
     *
     * @param config 服务配置参数
     * @param studyID 研究ID
     * @param sdkID SDKID
     * @param currentHashID 上一次调用返回的hashID，如果还没有数据，则返回空字符串
     * @param studyData 研究数据
     * @param hashID
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus retrieveStudyConfig(const CloudConfig& config,
                                            const std::string& studyID,
                                            const std::string& sdkID,
                                            const std::string& currentHashID,
                                            std::vector<uint8_t>& studyData,
                                            std::string& hashID);

    /**
     * \~english
     * @brief list study types
     *
     * @param config provides all the cloud configuration settings
     * @param status study status
     * @param studyTypes the returned list of study types on CLOUD_OK
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取研究类型
     *
     * @param config 服务配置参数
     * @param status 研究状态
     * @param studyTypes 研究类型信息
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus retrieveStudyTypes(const CloudConfig& config,
                                           const StudyStatus status,
                                           std::list<StudyType>& studyTypes);

    /**
     * \~english
     * @brief Retrieves a list of study templates that exist in a particular organization
     *
     * @param config provides all the cloud configuration settings
     * @param status study status
     * @param type study types
     * @param studyTemplates list of study templates
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取研究模板列表
     *
     * @param config 服务配置参数
     * @param status 研究状态
     * @param type 研究类型
     * @param studyTemplates 研究模板列表
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus listStudyTemplates(const CloudConfig& config,
                                           const StudyStatus status,
                                           const std::string& type,
                                           std::list<StudyTemplate>& studyTemplates);
};

} // namespace dfx::api

#endif // DFX_API_CLOUD_STUDY_API_H
