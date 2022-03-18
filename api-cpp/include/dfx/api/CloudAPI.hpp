// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_API_H
#define DFX_API_CLOUD_API_H

#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "dfx/api/CloudAPI_Export.hpp"

#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/CloudStatus.hpp"
#include "dfx/api/CloudTypes.hpp"

// Convenience for users so they won't need to pull in individually
#include "dfx/api/DeviceAPI.hpp"
#include "dfx/api/LicenseAPI.hpp"
#include "dfx/api/MeasurementAPI.hpp"
#include "dfx/api/MeasurementStreamAPI.hpp"
#include "dfx/api/OrganizationAPI.hpp"
#include "dfx/api/ProfileAPI.hpp"
#include "dfx/api/SignalAPI.hpp"
#include "dfx/api/StudyAPI.hpp"
#include "dfx/api/UserAPI.hpp"

namespace dfx::api
{

/**
 * @class CloudAPI CloudAPI.h "dfx/api/CloudAPI.h"
 *
 * \~english
 * @brief CloudAPI provides a C++ class which communicates with the Nuralogix
 *        DeepAffex API servers.
 *
 * There are different underlying channel transport formats which this Cloud
 * API abstracts from the application. As such, there are some calls which
 * result in an CLOUD_UNSUPPORTED_FEATURE error when a call is unable to be
 * fulfilled by the underlying channel implementation.
 *
 * Calls to CloudAPI take a CloudConfig structure which provides all the
 * properties necessary to make a request and returns a CloudStatus which
 * provides details of the request.
 *
 * \~chinese
 * @brief CloudAPI提供了与Nuralogix DeepAffex API服务器通信的C ++类。
 *
 * Cloud API从应用程序中抽象出了不同的底层通道传输格式。这样，
 * 当基础通道实现无法完成某个调用时，会有一些调用导致CLOUD_UNSUPPORTED_FEATURE错误。
 */
class DFXCLOUD_EXPORT CloudAPI : public std::enable_shared_from_this<CloudAPI>
{
protected:
    /**
     * \~english
     * @brief CloudAPI constructor.
     *
     * @param config parameters to use for communication with DeepAffex services
     * @see createInstance
     *
     * \~chinese
     * @brief CloudAPI构造函数。
     *
     * @param config 与DeepAffex服务进行通信的参数
     * @see createInstance
     */
    explicit CloudAPI(const CloudConfig& config);

    /**
     * \~english
     * @brief CloudAPI destructor.
     *
     * \~chinese
     * @brief CloudAPI析构函数。
     */
    virtual ~CloudAPI();

    virtual CloudStatus connect(const CloudConfig& config) = 0;

public:
    /**
     * \~english
     * The gRPC transport type (experimental type / available in some environments).
     *
     * \~chinese
     * gRPC传输类型（实验类型/在某些环境中可用）。
     */
    const static std::string TRANSPORT_TYPE_GRPC;

    /**
     * \~english
     * The WebSocket transport type.
     *
     * \~chinese
     * WebSocket传输类型。
     */
    const static std::string TRANSPORT_TYPE_WEBSOCKET;

    /**
     * \~english
     * The REST transport type.
     *
     * \~chinese
     * REST传输类型。
     */
    const static std::string TRANSPORT_TYPE_REST;

    /**
     * \~english
     * @brief Provides a list of all the transport types which are supported
     * by this CloudAPI library implementation.
     *
     * Different backend implementations are available depending upon how this
     * library was built. This is a helper method to identify which transports
     * are available at runtime.
     *
     * @return list of the available transports compiled into this library
     *
     * @see [User Guide: Transports](http://docs.deepaffex.ai/guide/cloud/1_transports.html)
     *
     * \~chinese
     * @brief 提供支持的所有传输类型的列表,通过这个CloudAPI库实现
     * 这是一个辅助方法来识别哪些传输在运行时可用
     *
     * @return 可用传输的列表
     *
     * @see (http://docs.deepaffex.ai/guide/cloud/1_transports.html)
     */
    static const std::list<std::string>& getAvailableTransports();

    /**
     * \~english
     * @brief Returns the version identifier of this library.
     *
     * @return version identifier for this library
     *
     * \~chinese
     * @brief 获取此库的版本号。
     *
     * @return 该库的版本标号
     */
    static const std::string& getVersion();

    /**
     * \~english
     * @brief get the type of device
     *
     * @return type of device
     *
     * \~chinese
     * @brief 获取设备类型。
     *
     * @return 设备类型
     */
    static const std::string& getPlatform();

    /**
     * \~english
     * @brief get the client identifier "CloudAPI"
     *
     * @return client identifier
     */
    static const std::string& getClientIdentifier();

    /**
     * \~english
     * @brief Get server status information, will attempt to connect.
     *
     * @param config parameters to use for communication with DeepAffex services
     * @return status of the operation
     * @see logout
     *
     * \~chinese
     * @brief 获取服务器的状态信息，状态和版本信息
     *
     * @param config 云端配置
     * @return CloudStatus
     */
    virtual CloudStatus getServerStatus(CloudConfig& config) = 0;

    /**
     * \~english
     * @brief Factory method to construct a CloudAPI which satisfies the conditions
     * of the CloudConfig.
     *
     * @param config provides all the cloud configuration settings
     * @param instance the created instance returned on CLOUD_OK
     * @return the CloudStatus of the request
     *
     * \~chinese
     * @brief 根据CloudConfig生成CloudAPI实例
     *
     * @param config 云端配置
     * @param instance CloudAPI实例
     * @return CloudStatus
     */
    static CloudStatus createInstance(CloudConfig& config, std::shared_ptr<CloudAPI>& instance);

    // *********************************************************************************
    // AUTHENTICATION SECTION
    // *********************************************************************************

    /**
     * \~english
     * @brief Login to the DFX Server obtaining a Config.m_sUserToken using the Config.m_sDeviceToken.
     *
     * @param config parameters to use for communication with DeepAffex services
     * @return status of the operation
     * @see logout
     *
     * \~chinese
     * @brief 登录服务,用户令牌将返回在Config.m_sUserToken
     *
     * @param config 云端配置
     * @return CloudStatus
     */
    virtual CloudStatus login(CloudConfig& config) = 0;

    /**
     * \~english
     * @brief Logout of the DFX Server clearing the Config.m_sUserToken on CLOUD_OK.
     *
     * @param config parameters to use for communication with DeepAffex services
     * @return status of the operation
     * @ see login
     *
     * \~chinese
     * @brief 登出服务
     *
     * @param config 云端配置参数
     * @return CloudStatus
     * @ see login
     */
    virtual CloudStatus logout(CloudConfig& config) = 0;

    /**
     * \~english
     * @brief Registers device with DFX Cloud API.
     *
     * @param config parameters to use for communication with DeepAffex services
     * @param appName
     * @param appVersion
     * @return
     * @see unregisterDevice
     *
     * \~chinese
     * @brief 注册设备
     *
     * @param config 云端配置参数
     * @param appName 设备名称(自定义)
     * @param appVersion 设备版本(x.x.xx)
     * @return CloudStatus
     * @see unregisterDevice
     */
    virtual CloudStatus registerDevice(CloudConfig& config,
                                       const std::string& appName,
                                       const std::string& appVersion) = 0;

    /**
     * \~english
     * @brief Unregister device from DFX Cloud API.
     *
     * @param config parameters to use for communication with DeepAffex services
     * @return status of the operation
     * @see registerDevice
     *
     * \~chinese
     * @brief 注销设备
     *
     * @param config 服务配置参数
     * @return CloudStatus
     * @see registerDevice
     */
    virtual CloudStatus unregisterDevice(CloudConfig& config) = 0;

    /**
     * \~english
     * @brief Validate the user token passed
     *
     * @param config parameters to use for communication with DeepAffex services
     * @param token
     * @return status of the operation
     * @see validateToken
     *
     * \~chinese
     * @brief 验证用户令牌
     *
     * @param config 服务配置参数
     * @param token	用户令牌
     * @return CloudStatus
     */
    virtual CloudStatus validateToken(const CloudConfig& config, const std::string& token) = 0;

    /**
     * \~english
     * @brief Switches the effective organization of the user if the user has permission to do so.
     *
     * @param config parameters to use for communication with DeepAffex services
     * @param organizationID the organization to switch to
     * @return status of the operation
     * @see validateToken
     */
    virtual CloudStatus switchEffectiveOrganization(CloudConfig& config, const std::string& organizationID) = 0;

    // *********************************************************************************
    // SERVICE SECTION
    // *********************************************************************************

    /**
     * \~english
     * @brief Obtains a handle to a DeviceAPI implementation.
     *
     * @param config parameters to use for communication with DeepAffex services
     * @return DeviceAPI handle which can be used to perform device operations. It will
     * never be a nullptr, but the returned handle may be limited in the device features
     * it supports.
     *
     * \~chinese
     * @brief 生成DeviceAPI实例
     *
     * @param config 服务配置参数
     * @return DeviceAPI指针,可与用设备相关接口调用
     */
    virtual std::shared_ptr<DeviceAPI> device(const CloudConfig& config);

    /**
     * \~english
     * @brief Obtains a handle to a LicenseAPI implementation.
     *
     * @param config parameters to use for communication with DeepAffex services
     * Not supported by gRPC backend.
     *
     * @return LicenseAPI handle which can be used to perform license operations. It will
     * never be a nullptr, but the returned handle may be limited in the license features
     * it supports.
     *
     * \~chinese
     * @brief 生成LicenseAPI实例
     *
     * @param config 服务配置参数,不支持GRPC
     *
     * @return LicenseAPI实例指针,可用于license相关接口调用
     */
    virtual std::shared_ptr<LicenseAPI> license(const CloudConfig& config);

    /**
     * \~english
     * @brief Obtains a handle to an MeasurementAPI implementation.
     *
     * @param config parameters to use for communication with DeepAffex services
     * @return MeasurementAPI handle which can be used to perform measurement
     * management operations. It will never be a nullptr, but the returned handle
     * may be limited in the measurement features it supports.
     *
     * @see measurementStream
     *
     * \~chinese
     * @brief 生成MeasurementAPI实例
     *
     * @param config 服务配置参数
     * @return MeasurementAPI实例指针,可用于measurement相关接口调用
     *
     * @see measurementStream
     */
    virtual std::shared_ptr<MeasurementAPI> measurement(const CloudConfig& config);

    /**
     * \~english
     * @brief OBtains a handle to a MeasurementStreamAPI implementation.
     *
     * @param config parameters to use for communication with DeepAffex services
     * @return MeasurementAPI handle which can be used to perform measurement
     * management operations. It will never be a nullptr, but the returned handle
     * may be limited in the measurement features it supports.
     *
     * \~chinese
     * @brief 生成MeasurementStreamAPI实例
     *
     * @param config 服务配置参数
     * @return MeasurementStreamAPI实例指针,用于measurementStream相关接口调用
     */
    virtual std::shared_ptr<MeasurementStreamAPI> measurementStream(const CloudConfig& config);

    /**
     * \~english
     * @brief Obtains a handle to an OrganizationAPI implementation.
     *
     * @param config parameters to use for communication with DeepAffex services
     * @return OrganizationAPI handle which can be used to perform organization
     * operations. It will never be a nullptr, but the returned handle may be limited
     * in the organization features it supports.
     *
     * \~chinese
     * @brief 生成OrganizationAPI实例
     *
     * @param config 服务配置参数
     * @return OrganizationAPI实例指针,用于organization相关接口
     */
    virtual std::shared_ptr<OrganizationAPI> organization(const CloudConfig& config);

    /**
     * \~english
     * @brief Obtains a handle to an ProfileAPI implementation.
     *
     * Not supported by gRPC backend.
     *
     * @param config parameters to use for communication with DeepAffex services
     * @return ProfileAPI handle which can be used to perform profile
     * operations. It will never be a nullptr, but the returned handle may be limited
     * in the profile features it supports.
     *
     * \~chinese
     * @brief 生成ProfileAPI实例,不支持GRPC
     *
     * @param config 服务配置参数
     * @return ProfileAPI实例指针,用于profile相关接口调用
     */
    virtual std::shared_ptr<ProfileAPI> profile(const CloudConfig& config);

    /**
     * \~english
     * @brief Obtains a handle to an SignalAPI implementation.
     *
     * Not supported by WEB backends.
     *
     * @param config parameters to use for communication with DeepAffex services
     * @return SignalAPI handle which can be used to perform signal operations. It will never
     * be a nullptr, but the returned handle may be limited in the signal features it supports.
     *
     * \~chinese
     * @brief 生成SignalAPI实例
     *
     * 不支持WEB方式
     *
     * @param config 服务配置参数
     * @return SignalAPI实例指针,用于signal相关接口调用
     */
    virtual std::shared_ptr<SignalAPI> signal(const CloudConfig& config);

    /**
     * \~english
     * @brief Obtains a handle to an StudyAPI implementation.
     *
     * @param config parameters to use for communication with DeepAffex services
     * @return StudyAPI handle which can be used to perform study operations. It will
     * never be a nullptr, but the returned handle may be limited in the study features
     * it supports.
     *
     * \~chinese
     * @brief 生成StudyAPI实例
     *
     * @param config 服务配置参数
     * @return StudyAPI实例指针,可用于study相关接口调用
     */
    virtual std::shared_ptr<StudyAPI> study(const CloudConfig& config);

    /**
     * \~english
     * @brief Obtains a handle to an UserAPI implementation.
     *
     * @param config parameters to use for communication with DeepAffex services
     * @return UserAPI handle which can be used to perform user operations. It will never be a
     * nullptr, but the returned handle may be limited in the user features it supports.
     *
     * \~chinese
     * @brief 生成UserAPI实例
     *
     * @param config 服务配置参数
     * @return UserAPI实例指针,用于user相关接口调用
     */
    virtual std::shared_ptr<UserAPI> user(const CloudConfig& config);

protected:
    /**
     * Helper function to retrieve a root CA.
     *
     * @param config potentially provides the root CA information
     * @return string containing the actual root CA content
     */
    static std::string getRootCA(const CloudConfig& config);

    /**
     * Counter for the number of outstanding CloudAPI instances, used inside the Curl Mutex
     * to ensure proper cleanup of the Curl global context.
     */
    static uint16_t numberCurlInstances;

    /**
     * Curl is thread safe, but has no internal thread synchronization. SSL libraries being used
     * may have additional requirements.
     *
     * @see https://curl.se/libcurl/c/threadsafe.html
     */
    static std::mutex curlMutex;

private:
    static std::string clientIdentifier;
};

} // namespace dfx::api

#endif // DFX_API_CLOUD_API_H
