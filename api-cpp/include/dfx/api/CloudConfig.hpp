// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_CONFIG_H
#define DFX_API_CLOUD_CONFIG_H

#include "dfx/api/CloudAPI_Export.hpp"

#include <ostream>
#include <string>

#include "CloudTypes.hpp"

namespace dfx::api
{
/**
 * @brief CloudConfig is a large structure of properties used by the CloudAPI when
 * performing different operations.
 *
 * Which not all values are required to make all calls, it is easier to group them
 * all into one structure (intentionally public) so that the client can load,
 * save and change values between calls as they choose without having to provide
 * individual API methods to address all the potential variants of optional
 * properties associated with a network connection. ie. secure/certs/timeouts.
 */
struct CloudConfig
{
public:
    /**
     * \~english
     * Context identifier used for loading and saving of CloudConfig files.
     *
     * Optional field and when empty, it is assumed to be "default". It is useful
     * when loading and storing different sets of configuration details in the same
     * configuration file but is not used for any API level requests.
     *
     * \~chinese
     * 用于加载和保存CloudConfig文件的上下文标识符
     *
     * 可选字段，如果为空，则假定为“默认值”。
     * 用于区分同一个配置文件,不同的配置信息集合
     */
    std::string contextID;

    /**
     * \~english
     * Transport type to use or empty to use the default / best available.
     *
     * If the connection to the server should use a particular transport type,
     * it should be identified here. Not all transports support all features and
     * not all servers or platforms support all features. This setting is generally
     * best left empty and the best available transport will be used.
     *
     * \~chinese
     * 传输类型,为空则适用最佳默认值
     *
     * 链接服务端的传输类型,不是所有类型都支持
     * 最好不去指定特定类型,API内部自主选择
     */
    std::string transportType;

    /**
     * \~english
     * The Authentication Email address.
     *
     * For User Login requests the auth email credential is required.
     *
     * \~chinese
     * 用户邮箱
     * 用户登录
     */
    std::string authEmail;

    /**
     * \~english
     * The Authentication Password.
     *
     * For User Login requests the auth password credential is required.
     *
     * \~chinese
     * 密码
     */
    std::string authPassword;

    /**
     * \~english
     * The Authentication Organization.
     *
     * For User Login requests the auth organization is required.
     *
     * \~chinese
     * 组织名称
     */
    std::string authOrg;

    /**
     * \~english
     * The Organization License.
     *
     * For Device Registration, the Organization License is required.
     *
     * \~chinese
     * 用户凭证
     *
     * 用户设备注册
     */
    std::string license;

    /**
     * \~english
     * The server host name to communicate with.
     *
     * This could be a DFX cloud service or a standalone server capable of providing
     * the DFX API communication facilities.
     *
     * \~chinese
     * 服务端地址
     */
    std::string serverHost;

    /**
     * \~english
     * Optionally defined server port to communicate with.
     *
     * This is used in conjunction with the m_sServerHost to connect with the
     * DFX API. All implementations have defaults for well known ports so this
     * field can be left empty, but if your service is running on a non-standard
     * port this may be required.
     *
     * \~chinese
     * 服务端口
     * 通常服务端端口都是默认固定的，可以不指定
     * 除非使用的是本地，手动配置过不同端口的服务
     */
    uint16_t serverPort;

    /**
     * \~english
     * The Study ID is required when performing measurements.
     *
     * The Study ID identifies to the server which algorithms need to be run and
     * which signals generated when a chunk is sent to the server.
     *
     * \~chinese
     * 研究ID,创建测试时需要
     * 研究指明了会有哪些信号
     */
    std::string studyID;

    /**
     * \~english
     * The Root Certificate of the server to help establish a TLS session.
     *
     * Typically this is not required and can remain blank, but for standalone
     * servers or servers which rely on self-signed certificates this can be
     * use to provide the necessary certificate.
     *
     * \~chinese
     * TLS会话的服务器的根证书
     *
     * 通常这是不需要的，可以保持空白，但对于独立的
     * 依赖自签名证书的服务器或服务器
     * 用于提供必要的证书
     */
    std::string rootCA;

    /**
     * \~english
     * The Authentication Token obtained by the DFX User Login request.
     *
     * This is a JWT token which can be re-used across sessions provided it has
     * not expired or otherwise becomes invalid. Almost all server requests
     * require some form of JWT token to be presented to validate the user
     * making the request.
     *
     * \~chinese
     * 身份验证令牌。
     *
     * 这是一个JWT令牌，只要它具有
     * 未过期或无效。
     * 几乎所有的接口都需要令牌来进行身份验证
     */
    std::string authToken;

    /**
     * \~english
     * The Device Token obtained by the DFX Device Register request.
     *
     * This is a JWT token which can be re-used across sessions provided it has
     * not expired or otherwise becomes invalid. This token is *ONLY* required
     * when making measurements and identifies the device making the
     * request.
     *
     * The device token should be obtained before the User login token is acquired
     * to ensure the User login AuthToken incorporates the necessary device details.
     *
     * \~chinese
     * 设备令牌。
     * 仅在进行测量和识别设备时此令牌仅必需
     * 应在获取用户登录令牌之前获取设备令牌
     * 以确保用户登录AuthToken包含必要的设备详细信息。
     */
    std::string deviceToken;

    /**
     * \~english
     * The Device ID obtained by the DFX Device Register request.
     *
     * In addition to a Device Token, the Device Register request provides this
     * device ID which can be used to Unregister the Device Token. Most licenses
     * will include a limit on the number of outstanding Device Tokens and so
     * devices, identified by their device ID should be unregistered if they
     * are no longer required.
     *
     * \~chinese
     * 设备ID。
     * 除了设备令牌之外，设备注册请求还提供了
     * 可用于注销设备令牌的设备ID
     */
    std::string deviceID;

    /**
     * \~english
     * The network timeout in milliseconds to use for server communications.
     *
     * \~chinese
     * 链接服务超时时间,单位毫秒
     */
    uint16_t timeoutMillis;

    /**
     * \~english
     * The default list limit to use when retrieving a page of information.
     *
     * By default, it is 25.
     *
     * \~chinese
     * 检索信息时,一次请求的默认列表限制。
     * 默认25
     */
    uint16_t listLimit;

    /**
     * \~english
     * Secure is used to identify if the server connection should be protected by TLS
     * or will be using an unsecure channel.
     *
     * This defaults to true and must explicitly be configured as false to take
     * effect. Very rarely would this ever be false, all production servers use TLS
     * and do not support unsecure channels.
     *
     * \~chinese
     * secure用于标识服务器连接是否应受到TLS的保护
     * 或者将使用不安全的通道。
     * 默认true，false需要显式配置
     * 所有的服务器都使用TLS，不支持不安全通道
     */
    bool secure;

    /**
     * \~english
     * When a TLS session is being established, some protocols can optionally skip
     * verifying the server.
     *
     * This is by default false and very rarely would be used since the validity of
     * the remote sever connection might be compromised. It is useful when you know
     * you trust the connection and are perhaps using a different host name then
     * the server is known by. Not required for any production server.
     *
     * \~chinese
     * 是否进行服务安全验证
     * 当很明确知道服务安全的情况下，可跳过验证
     */
    bool skipVerify;
};

/**
 * \~english
 * @brief Load a configuration file containing the information necessary
 * to connect to the DFX Cloud API.
 *
 * @param config the retrieved config information on CLOUD_OK
 * @param filePath the location on disk for where the dfxcloud.yml file is located
 * @return status of operation, CLOUD_OK on SUCCESS
 *
 * \~chinese
 * @brief 加载服务配置信息，登录链接服务所需的信息
 *
 * @param config 服务配置信息
 * @param filePath 配置文件地址(dfxcloud.yml)
 * @return CloudStatus
 */
DFXCLOUD_EXPORT CloudStatus loadCloudConfig(CloudConfig& config, const std::string& filePath = "~/.dfxcloud.yaml");

DFXCLOUD_EXPORT CloudStatus getAvailableContexts(const std::string& filePath,
                                                 std::string& defaultContext,
                                                 std::vector<std::string>& contextNames);

} // namespace dfx::api

DFXCLOUD_EXPORT std::ostream& operator<<(std::ostream& os, const dfx::api::CloudConfig& config);

#endif // DFX_API_CLOUD_CONFIG_H
