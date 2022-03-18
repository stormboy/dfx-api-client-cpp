// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_USER_API_H
#define DFX_API_CLOUD_USER_API_H

#include "dfx/api/CloudAPI_Export.hpp"
#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/CloudStatus.hpp"
#include "dfx/api/types/UserTypes.hpp"

#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <string>

namespace dfx::api
{

class CloudAPI;

class DFXCLOUD_EXPORT UserAPI
{
public:
    enum class UserFilter
    {
        FullObject
    };

    /**
     * @brief UserAPI constructor.
     */
    UserAPI() = default;

    /**
     * @brief UserAPI destructor.
     */
    virtual ~UserAPI() = default;

    /**
     * \~english
     * @brief Create a user on the DFX Cloud.
     *
     * A JWT User Token is required to perform operations with the DFX Cloud, to
     * obtain a User Token a user must exist on the system and login using their
     * username and password combination.
     *
     * Only a user with the administrator role for an organization can create
     * a user account.
     *
     * @param config provides all the cloud configuration settings
     * @param email the email address for the account being created
     * @param password the password associated with the newly created account
     * @param role the role the created user will have
     * @param firstName (Optional)the firstName the created user will have
     * @param lastName (Optional)the lastName the created user will have
     * @param phoneNumber (Optional)user phoneNumber
     * @param gender (Optional)user gender
     * @param dateOfBirth (Optional)user dateOfBirth "YYYY-MM-DD"
     * @param heightCM (Optional)user height
     * @param weightKG (Optional)user weight
     * @param userID the unique identifier for the created account
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 创建用户
     *
     * 在组织中创建一个新用户，并根据用户列表检查现有的用户详细信。
     * Email + Password OR PhoneNumber字段为必填项。
     * 如果两者都提供，Email + Password将用于创建用户帐户。其余字段是可选的
     *
     * 只有具有组织的管理员角色的用户才能创建用户帐户.
     *
     * @param config 服务配置参数
     * @param email 邮箱
     * @param password 密码
     * @param role 角色
     * @param firstName (可选)名
     * @param lastName (可选)姓
     * @param phoneNumber (可选)电话号码
     * @param gender (可选)性别
     * @param dateOfBirth (可选)生日 "YYYY-MM-DD"
     * @param heightCM (可选)身高
     * @param weightKG (可选)体重
     * @param userID 用户ID
     * @return 接口返回值 CLOUD_OK:成功
     *
     */
    virtual CloudStatus create(const CloudConfig& config,
                               const std::string& email,
                               const std::string& password,
                               const std::string& role,
                               const std::string& firstName,
                               const std::string& lastName,
                               const std::string& phoneNumber,
                               const std::string& gender,
                               const std::string& dateOfBirth,
                               const uint16_t heightCM,
                               const uint16_t weightKG,
                               std::string& userID);

    /**
     * \~english
     * @brief Retrieve the users visible to the logged in user on the
     * DFX Cloud server.
     *
     * @param config provides all the cloud configuration settings
     * @param filters
     * @param offset to start the retrieval of records up to config.listLimit
     * @param users the vector where the returned users will be added
     * @param totalCount the total number of records on the server
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取用户信息
     *
     * @param config 服务配置参数
     * @param filters
     * @param offset 偏移量指定查询开始位置
     * @param users 用户信息
     * @param totalCount 用户总数
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus list(const CloudConfig& config,
                             const std::unordered_map<UserFilter, std::string>& filters,
                             uint16_t offset,
                             std::vector<User>& users,
                             int16_t& totalCount);

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
    retrieve(const CloudConfig& config, const std::string& userID, const std::string& email, User& user);

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
    virtual CloudStatus update(const CloudConfig& config, const User& user);

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
    virtual CloudStatus remove(const CloudConfig& config, const std::string& userID, const std::string& email);

    /**
     * \~english
     * @brief get login verification code.
     *
     * Use mobile phone number to get login verification code
     * First of all, the mobile phone number is the number of the registered user
     * The obtained code will be notified by SMS
     *
     * @param config provides all the cloud configuration settings
     * @param sOrgKey User organization name
     * @param sPhoNum Phone number
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取登录验证码
     *
     * 用电话获取登录验证码
     * 首先，手机号码是注册用户的号码
     * 获得的验证码将通过短信通知
     *
     * @param config 服务配置参数
     * @param sOrgKey 组织标识
     * @param sPhoNum 电话号码
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus reqLoginCode(const CloudConfig& config, const std::string& sOrgKey, const std::string& sPhoNum);

    /**
     * \~english
     * @brief login with verification code
     *
     * @param config provides all the cloud configuration settings
     * @param sOrgKey User organization name
     * @param sPhoNum Phone number
     * @param sPhoCode verification code
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 通过验证码登录
     *
     * @param config 服务配置参数
     * @param sOrgKey 组织标识
     * @param sPhoNum 电话号码
     * @param sPhoCode 验证码
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus loginWithPhoneCode(CloudConfig& config,
                                           const std::string& sOrgKey,
                                           const std::string& sPhoNum,
                                           const std::string& sPhoCode);

    /**
     * \~english
     * @brief Get the current user role information
     *
     * @param config provides all the cloud configuration settings
     * @param userRole the retrieved user role
     * @return status of operation, CLOUD_OK on SUCCESS
     *
     * \~chinese
     * @brief 获取角色信息
     *
     * @param config 服务配置参数
     * @param userRole 角色信息
     * @return 接口返回值 CLOUD_OK:成功
     */
    virtual CloudStatus retrieveUserRole(const CloudConfig& config, UserRole& userRole);

    /**
     * \~english
     * @brief Password reset request
     *
     *Dispatches a password reset request email to the user which contains a reset password token.
     *Upon retrieving this value the token is used against the Reset Password endpoint
     *
     * @param config provides all the cloud configuration settings
     * @param sIdentifier Organization identification
     * @param sResetToken the retrieved Reset Token
     * @return status of operation, CLOUD_OK on SUCCESS
     * @see resetPassword
     *
     * \~chinese
     * @brief 请求重置密码
     *
     * 向包含重置密码令牌的用户发送密码重置请求电子邮件
     *
     * @param config 服务配置参数
     * @param sIdentifier 组织标识
     * @param sResetToken 重置令牌
     * @return 接口返回值 CLOUD_OK:成功
     * @see resetPassword
     */
    virtual CloudStatus sendPasswordReset(const CloudConfig& config,
                                          const std::string& sIdentifier,
                                          std::string& sResetToken);

    /**
     * \~english
     * @brief reset password
     *
     *
     * @param config provides all the cloud configuration settings
     * @param sPassword New password
     * @param sResetToken Reset Token
     * @return status of operation, CLOUD_OK on SUCCESS
     * @see sendPasswordReset
     *
     * \~chinese
     * @brief reset password
     *
     *
     * @param config 服务配置参数
     * @param sPassword 密码
     * @param sResetToken 重置令牌
     * @return 接口返回值 CLOUD_OK:成功
     * @see sendPasswordReset
     */
    virtual CloudStatus resetPassword(const CloudConfig& config,
                                      const std::string& sPassword,
                                      const std::string& sResetToken);

    /**
     * \~english
     * @brief Sends an account verification code to the user's email address
     *
     * @param config provides all the cloud configuration settings
     * @param sUserID user id
     * @param sOrgKey User organization id
     * @return status of operation, CLOUD_OK on SUCCESS
     * @see verifyAccount
     *
     * \~chinese
     * @brief 将帐户验证码发送到用户的电子邮件地址
     *
     * @param config 服务配置参数
     * @param sUserID 用户ID
     * @param sOrgKey 组织标识
     * @return 接口返回值 CLOUD_OK:成功
     * @see verifyAccount
     */
    virtual CloudStatus sendVerificationCode(const CloudConfig& config,
                                             const std::string& sUserID,
                                             const std::string& sOrgKey);

    /**
     * \~english
     * @brief reset password
     *
     * @param config provides all the cloud configuration settings
     * @param sUserID user id
     * @param sVerCode Verification Code
     * @return status of operation, CLOUD_OK on SUCCESS
     * @see sendVerificationCode
     *
     * \~chinese
     * @brief 重置密码
     *
     * @param config 服务配置参数
     * @param sUserID 用户ID
     * @param sVerCode 验证码
     * @return 接口返回值 CLOUD_OK:成功
     * @see sendVerificationCode
     */
    virtual CloudStatus verifyAccount(const CloudConfig& config,
                                      const std::string& sUserID,
                                      const std::string& sVerCode);
};

} // namespace dfx::api

#endif // DFX_API_CLOUD_USER_API_H
