// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_PROFILE_API_H
#define DFX_API_CLOUD_PROFILE_API_H

#include "dfx/api/CloudAPI_Export.hpp"
#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/CloudStatus.hpp"
#include "dfx/api/types/ProfileTypes.hpp"

#include <cstdint>
#include <list>
#include <memory>
#include <string>

namespace dfx::api
{

class CloudAPI;

/**
 * \~english
 * Profiles are elements of user accounts. A single user account may maintain a number
 * of profiles to help segment different types of users.
 *
 * \~chinese
 * 配置文件是用户帐户的元素。
 * 一个用户帐户可以维护多个配置文件,以帮助细分不同类型的用户
 */
class DFXCLOUD_EXPORT ProfileAPI
{
public:
    enum class ProfileFilter
    {
        FullObject,
        AccountId,
        ProfileName,
        ProfileStatus,
        UserEmail
    };

    /**
     * @brief ProfileAPI constructor.
     */
    ProfileAPI() = default;

    /**
     * @brief ProfileAPI destructor.
     */
    virtual ~ProfileAPI() = default;

    /**
     * \~english
     * @brief Creates a user profile under a main user account.
     *
     * User profiles may be used to segment different accounts for measurements.
     *
     * @param config provides all the cloud configuration settings
     * @param name
     * @param email
     * @param profile the newly created profile returned on CLOUD_OK
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 创建配置文件
     *
     * 用户配置文件可用于划分测量的不同帐户.
     *
     * @param config 服务配置参数
     * @param name 名称
     * @param email	邮箱
     * @param profile 配置文件ID
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus
    create(const CloudConfig& config, const std::string& name, const std::string& email, Profile& profile);

    /**
     * \~english
     * @brief Lists specific profiles managed under the current user account.
     *
     * @param config provides all the cloud configuration settings
     * @param filters
     * @param offset to start the retrieval of records up to config.listLimit
     * @param profiles
     * @param totalCount the total number of records on the server
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取配置文件列表
     *
     * @param config 服务配置参数
     * @param filters
     * @param offset 偏移量指定查询开始位置
     * @param profiles 配置文件列表
     * @param totalCount 配置文件总数
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus list(const CloudConfig& config,
                             const std::unordered_map<ProfileFilter, std::string>& filters,
                             uint16_t offset,
                             std::vector<Profile>& profiles,
                             int16_t& totalCount);

    /**
     * \~english
     * @brief Retrieves a single user Profile specified by ID. If the
     * record is found the other fields of the profile instance will be returned.
     *
     * @param config provides all the cloud configuration settings
     * @param profileID
     * @param profile
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取配置文件
     *
     * @param config 服务配置参数
     * @param profileID
     * @param profile
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus retrieve(const CloudConfig& config, const std::string& profileID, Profile& profile);

    /**
     * \~english
     * @brief Updates the status of a specific user profile.
     *
     * @param config provides all the cloud configuration settings
     * @param profile specifies the profile and status
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 更新配置文件
     *
     * @param config 服务配置参数
     * @param profile  配置文件信息
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus update(const CloudConfig& config, const Profile& profile);

    /**
     * \~english
     * @brief Removes the user profile entirely.
     *
     * It also deletes any related meta fields associated with the profile.
     *
     * @param config provides all the cloud configuration settings
     * @param profileID the id of the profile to remove
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 删除配置文件
     *
     * 它还会删除与配置文件关联的所有相关元字段
     *
     * @param config 服务配置参数
     * @param profileID	配置文件ID
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus remove(const CloudConfig& config, const std::string& profileID);
};

} // namespace dfx::api

#endif // DFX_API_CLOUD_PROFILE_API_H
