// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_ORGANIZATION_API_H
#define DFX_API_CLOUD_ORGANIZATION_API_H

#include "UserAPI.hpp"
#include "dfx/api/CloudAPI_Export.hpp"
#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/CloudStatus.hpp"
#include "dfx/api/types/OrganizationTypes.hpp"
#include "dfx/api/types/UserTypes.hpp"

#include <cstdint>
#include <list>
#include <memory>
#include <string>

namespace dfx::api
{

class CloudAPI;

/**
 * \~english
 * @brief Organizational endpoints. All endpoints require Researcher, Lead or Admin access.
 *
 * \~chinese
 * @brief 组织接口,所有接口都需要研究员、主管或管理员访问权限
 */
class DFXCLOUD_EXPORT OrganizationAPI
{
public:
    enum class OrganizationFilter
    {
        FullObject
    };

    /**
     * @brief OrganizationAPI constructor.
     */
    OrganizationAPI() = default;

    /**
     * @brief OrganizationAPI destructor.
     */
    virtual ~OrganizationAPI() = default;

    /**
     * \~english
     * @brief create Organization
     *
     * @param config provides all the cloud configuration settings
     * @param name a descriptive name to associate with the organization
     * @param identifier a UUID identifier for the organization
     * @param public_key the Organization License is required
     * @param status the Organization status
     * @param logo the Organization logo
     * @organizationID the newly created organization identifier returned on CLOUD_OK
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 创建组织
     *
     * @param config 服务配置参数
     * @param identifier 组织标识
     * @param public_key 组织许可证
     * @param status 状态
     * @param logo 组织图标
     * @organizationID 组织ID
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus create(const CloudConfig& config,
                               const std::string& name,
                               const std::string& identifier,
                               const std::string& public_key,
                               const OrganizationStatus& status,
                               const std::string& logo,
                               std::string& organizationID);

    /**
     * \~english
     * @brief Retrieves a list of existing organization
     *
     * @param config provides all the cloud configuration settings
     * @param offset to start the retrieval of records up to config.listLimit
     * @param organizations list of organization being returned
     * @param totalCount the total number of records on the server
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取组织列表
     *
     * @param config 服务配置参数
     * @param offset 偏移量指定查询开始位置
     * @param organizations 组织列表
     * @param totalCount 总数
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus list(const CloudConfig& config,
                             const std::unordered_map<OrganizationFilter, std::string>& filters,
                             uint16_t offset,
                             std::vector<Organization>& organizations,
                             int16_t& totalCount);

    /**
     * \~english
     * @brief Retrieves a existing organization
     *
     * @param config provides all the cloud configuration settings
     * @param organizationID the organization ID to retrieve
     * @param organization the retrieved organization information
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取组织信息
     *
     * @param config 服务配置参数
     * @param organizationID 组织ID
     * @param organization 组织信息
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus retrieve(const CloudConfig& config,
                                 const std::string& organizationID,
                                 Organization& organization);

    /**
     * \~english
     * @brief Retrieve full organization details for a organizations vector.
     *
     * @param config provides all the cloud configuration settings
     * @param organizationIDs list of organization IDs to obtain details for
     * @param organizations list of organizations being returned
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 批量获取组织
     *
     * @param config 服务配置参数
     * @param organizationIDs 组织ID
     * @param organizations 组织信息列表
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus retrieveMultiple(const CloudConfig& config,
                                         const std::vector<std::string>& organizationIDs,
                                         std::vector<Organization>& organizations);

    /**
     * \~english
     * @breif Update an existing organization based on the provided organizationID.
     *
     * @param config provides all the cloud configuration settings
     * @param organization details to update for the organization
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @breif 更新组织
     *
     * @param config 服务配置参数
     * @param organization 组织信息
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus update(const CloudConfig& config, Organization& organization);

    /**
     * \~english
     * @brief Remove an existing organization based on the provided organizationID.
     *
     * @param config provides all the cloud configuration settings
     * @param organizationID the organization identifier to remove
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 删除组织
     *
     * @param config 服务配置参数
     * @param organizationID 组织ID
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus remove(const CloudConfig& config, const std::string& organizationID);

    /**
     * \~english
     * @brief Retrieves a list of users within the organization
     *
     * @param config provides all the cloud configuration settings
     * @param offset to start the retrieval of records up to config.listLimit
     * @param users list of user being returned
     * @param totalCount the total number of records on the server
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取组织列表
     *
     * @param config 服务配置参数
     * @param offset 偏移量指定查询开始位置
     * @param users 组织列表
     * @param totalCount 总数
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus listUsers(const CloudConfig& config,
                             const std::unordered_map<dfx::api::UserAPI::UserFilter, std::string>& filters,
                             uint16_t offset,
                             std::vector<User>& users,
                             int16_t& totalCount);

    /**
     * \~english
     * @brief Create a users within the organization
     *
     * @param config provides all the cloud configuration settings
     * @param user the user properties to create a user with, server will update properties returned on CLOUD_OK
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取组织列表
     *
     * @param config 服务配置参数
     * @param user
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus createUser(const CloudConfig& config, User& user);

    /**
     * \~english
     * @brief Retrieve a user from the server based on the userID and/or email.
     *
     * \verbatim embed:rst:leading-asterisk
     *     .. note::
     *
     *        Only one of userID or email is required and the other can be
     *        left empty except in the case where the email is used across
     *        multiple organizations and the userID is required.
     *
     * \endverbatim
     *
     * @param config provides all the cloud configuration settings
     * @param userID unique ID of the user being retrieved
     * @param email the associated email address of the account
     * @param user the retrieved user account
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取用户
     *
     * @param config 服务配置参数
     * @param userID 用户ID
     * @param email the 邮箱
     * @param user 用户信息
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus
    retrieveUser(const CloudConfig& config, const std::string& userID, const std::string& email, User& user);

    /**
     * \~english
     * @brief Update a user on the server.
     *
     * @param config provides all the cloud configuration settings
     * @param user account to update
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 更新用户
     *
     * @param config 服务配置参数
     * @param user 用户信息
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus
    updateUser(const CloudConfig& config, const std::string& userID, const std::string& email, const User& user);

    /**
     * \~english
     * @brief Remove a user from the server based on the userID and/or email.
     *
     * \verbatim embed:rst:leading-asterisk
     *     .. note::
     *
     *        Only one of userID or email is required and the other can be
     *        left empty except in the case where the email is used across
     *        multiple organizations and the userID is required.
     *
     * \endverbatim
     *
     * @param config provides all the cloud configuration settings
     * @param userID unique ID of the user being retrieved
     * @param email the associated email address of the account
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 删除用户
     *
     * @param config 服务配置参数
     * @param userID 用户ID
     * @param email 邮箱
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus removeUser(const CloudConfig& config, const std::string& userID, const std::string& email);
};

} // namespace dfx::api

#endif // DFX_API_CLOUD_ORGANIZATION_API_H
