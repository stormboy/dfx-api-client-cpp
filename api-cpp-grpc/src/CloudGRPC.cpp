// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/auth/v1/auth.grpc.pb.h"
#include "dfx/devices/v2/devices.grpc.pb.h"
#include "dfx/measurements/v2/measurements.grpc.pb.h"

#include "dfx/api/OrganizationAPI.hpp"

#include "dfx/api/grpc/CloudGRPC.hpp"
#include "dfx/api/grpc/DeviceGRPC.hpp"
#include "dfx/api/grpc/MeasurementGRPC.hpp"
#include "dfx/api/grpc/MeasurementStreamGRPC.hpp"
#include "dfx/api/grpc/SignalGRPC.hpp"
#include "dfx/api/grpc/StudyGRPC.hpp"
#include "dfx/api/grpc/UserGRPC.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

#include <ctime>
#include <fmt/format.h>
#include <google/protobuf/util/time_util.h>
#include <memory>

// GRPC_VERBOSITY=DEBUG;GRPC_TRACE=all

// VisualStudio resolution is not as nice as Clang and gets confused by the
// ::grpc and dfx::api::grpc when pulling in the dfx::api namespace all at once.
using dfx::api::CloudAPI;
using dfx::api::CloudConfig;
using dfx::api::CloudStatus;
using dfx::api::DeviceAPI;
using dfx::api::MeasurementAPI;
using dfx::api::MeasurementStreamAPI;
using dfx::api::OrganizationAPI;
using dfx::api::ProfileAPI;
using dfx::api::SignalAPI;
using dfx::api::StudyAPI;
using dfx::api::UserAPI;

using namespace dfx::api::grpc;

using ::grpc::ClientContext;

#define MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(call)                                                                 \
    {                                                                                                                  \
        ::grpc::Status status = call;                                                                                  \
        if (!status.ok()) {                                                                                            \
            return translateGrpcStatus(status);                                                                        \
        }                                                                                                              \
    }

CloudGRPC::CloudGRPC(const CloudConfig& config) : CloudAPI(config)
{
    // Verifies that we have not accidentally linked against a version of the library which is incompatible
    // with the version of the headers we compiled with. This is a ***PROGRAM ABORT*** - it means something
    // very wrong with our build environment.
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

CloudStatus CloudGRPC::connect(const CloudConfig& config)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, connect(config));

    return CloudStatus(CLOUD_OK);
}

void CloudGRPC::initializeClientContext(const CloudConfig& config,
                                        ::grpc::ClientContext& context,
                                        const std::string& token)
{
    setAuthTokenClientContext(config, context, token);
    setDeadlineClientContext(config, context);
}

void CloudGRPC::setDeadlineClientContext(const CloudConfig& config, ::grpc::ClientContext& context)
{
    gpr_timespec ts;
    ts.tv_sec = config.timeoutMillis / 1000;
    ts.tv_nsec = static_cast<int32_t>((config.timeoutMillis - static_cast<uint16_t>(ts.tv_sec * 1000)) * 1e6);
    ts.clock_type = GPR_TIMESPAN;
    context.set_deadline(ts);
}

void CloudGRPC::setAuthTokenClientContext(const CloudConfig& config,
                                          ::grpc::ClientContext& context,
                                          const std::string& token)
{
    if (token.length() > 0) {
        context.AddMetadata("authorization", "Bearer " + token);
    }
}

std::string CloudGRPC::getServerURL(const std::string& hostname, int port)
{
    return hostname + ":" + std::to_string(port);
}

std::string grpcCodeToString(const ::grpc::Status& status)
{
    std::string grpcCode;
    switch (status.error_code()) {
        case ::grpc::StatusCode::OK:
            return "OK";
        case ::grpc::StatusCode::CANCELLED:
            return "CANCELLED";
        case ::grpc::StatusCode::UNKNOWN:
            return "UNKNOWN";
        case ::grpc::StatusCode::INVALID_ARGUMENT:
            return "INVALID_ARGUMENT";
        case ::grpc::StatusCode::DEADLINE_EXCEEDED:
            return "DEADLINE_EXCEEDED";
        case ::grpc::StatusCode::NOT_FOUND:
            return "NOT_FOUND";
        case ::grpc::StatusCode::ALREADY_EXISTS:
            return "ALREADY_EXISTS";
        case ::grpc::StatusCode::PERMISSION_DENIED:
            return "PERMISSION_DENIED";
        case ::grpc::StatusCode::UNAUTHENTICATED:
            return "UNAUTHENTICATED";
        case ::grpc::StatusCode::RESOURCE_EXHAUSTED:
            return "RESOURCE_EXHAUSTED";
        case ::grpc::StatusCode::FAILED_PRECONDITION:
            return "FAILED_PRECONDITION";
        case ::grpc::StatusCode::ABORTED:
            return "ABORTED";
        case ::grpc::StatusCode::OUT_OF_RANGE:
            return "OUT_OF_RANGE";
        case ::grpc::StatusCode::UNIMPLEMENTED:
            return "UNIMPLEMENTED";
        case ::grpc::StatusCode::INTERNAL:
            return "INTERNAL";
        case ::grpc::StatusCode::UNAVAILABLE:
            return "UNAVAILABLE";
        case ::grpc::StatusCode::DATA_LOSS:
            return "DATA_LOSS";
        case ::grpc::StatusCode::DO_NOT_USE:
            return "DO_NOT_USE";
    }
    return "INVALID_CODE";
}

CloudStatus CloudGRPC::translateGrpcStatus(const ::grpc::Status& status)
{
    auto errorMessage = status.error_message();
    std::string dfxErrorCodeString("dfx-error-code: ");
    std::string dfxMessageString(", message: ");
    std::size_t foundDfxError = errorMessage.find(dfxErrorCodeString);
    if (foundDfxError != std::string::npos) {
        auto dfxErrorWithCode = errorMessage.substr(foundDfxError + dfxErrorCodeString.length());
        std::size_t foundDfxMessage = dfxErrorWithCode.find(dfxMessageString);
        if (foundDfxMessage != std::string::npos) {
            auto dfxMessage = dfxErrorWithCode.substr(foundDfxMessage + dfxMessageString.length());
            auto dfxErrorCode = dfxErrorWithCode.substr(0, foundDfxMessage);
            errorMessage = dfxMessage;
            if (errorMessage.empty()) {
                errorMessage = dfxErrorCode;
            }
        }
    }

    dfx_status_code cloudStatus(CLOUD_TRANSPORT_FAILURE);
    switch (status.error_code()) {
        case ::grpc::StatusCode::OK:
            cloudStatus = CLOUD_OK;
            break;
        case ::grpc::StatusCode::INVALID_ARGUMENT:
            cloudStatus = CLOUD_BAD_REQUEST;
            break;
        case ::grpc::StatusCode::NOT_FOUND:
            cloudStatus = CLOUD_RECORD_NOT_FOUND;
            break;
        case ::grpc::StatusCode::UNIMPLEMENTED:
            cloudStatus = CLOUD_UNIMPLEMENTED_FEATURE;
            break;
        case ::grpc::StatusCode::UNAUTHENTICATED:
            cloudStatus = CLOUD_USER_NOT_AUTHENTICATED;
            break;
        case ::grpc::StatusCode::FAILED_PRECONDITION:
            cloudStatus = CLOUD_PARAMETER_VALIDATION_ERROR;
            break;
        case ::grpc::StatusCode::ALREADY_EXISTS:
            cloudStatus = CLOUD_RECORD_ALREADY_EXISTS;
        default:
            break;
    };

    return CloudStatus(cloudStatus, errorMessage, status.error_code(), status.error_message());
}

// *********************************************************************************
// AUTHENTICATION SECTION
// *********************************************************************************

CloudStatus CloudGRPC::login(CloudConfig& config)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, login(config));

    dfx::auth::v1::LoginResponse response;
    dfx::auth::v1::LoginRequest request;
    request.set_email(config.authEmail);
    request.set_password(config.authPassword);
    request.set_organization_identifier(config.authOrg);

    auto pAuth = dfx::auth::v1::API::NewStub(getChannel(config));

    // Login is special, we can use it to "upgrade" our device token so we can
    // perform measurements, or we can just login and get a user token.
    ClientContext context;
    if (!config.deviceToken.empty()) {
        // We have a device token, so UPGRADE the token
        initializeClientContext(config, context, config.deviceToken);
    } else {
        initializeClientContext(config, context, "");
    }

    {
        ::grpc::Status status = pAuth->Login(&context, request, &response);
        if (!status.ok()) {
            auto dfxStatus = translateGrpcStatus(status);
            if (dfxStatus.code == CLOUD_USER_NOT_AUTHENTICATED) {
                dfxStatus.message = "Invalid username or password";
            }
            return dfxStatus;
        }
    }

    config.authToken = response.token();

    return CloudStatus(CLOUD_OK);
}

CloudStatus CloudGRPC::logout(CloudConfig& config)
{
    dfx::auth::v1::LogoutResponse response;
    dfx::auth::v1::LogoutRequest request;

    auto pAuth = dfx::auth::v1::API::NewStub(getChannel(config));

    ClientContext context;
    initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(pAuth->Logout(&context, request, &response));

    return CloudStatus(CLOUD_OK);
}

CloudStatus CloudGRPC::registerDevice(CloudConfig& config, const std::string& appName, const std::string& appVersion)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, registerDevice(config, appName, appVersion));

    auto grpcDevices = dfx::devices::v2::API::NewStub(getChannel(config));

    dfx::devices::v2::RegisterResponse response;
    dfx::devices::v2::RegisterRequest request;
    request.set_license_id(config.license);

    request.set_name(appName);
    request.set_version(appVersion);

    request.set_identifier(CloudAPI::getClientIdentifier() + CloudAPI::getVersion());

    // Try to guess a reasonable device type based on the platform the library was compiled for.
    const std::string platformID = CloudAPI::getPlatform();

    if (platformID.rfind("Windows", 0) == 0) {
        request.set_device_type(dfx::devices::v2::Type::WIN32_DEPRECATED); // Missing option for WIN64?
    } else if (platformID.rfind("Linux", 0) == 0) {
        request.set_device_type(dfx::devices::v2::Type::LINUX);
    } else if (platformID.rfind("Mac", 0) == 0) {
        request.set_device_type(dfx::devices::v2::Type::DARWIN);
    } else if (platformID.rfind("iOS", 0) == 0) {
        request.set_device_type(dfx::devices::v2::Type::IPHONE);
    } else if (platformID.rfind("Android", 0) == 0) {
        request.set_device_type(dfx::devices::v2::Type::ANDROID_PHONE);
    } else {
        request.set_device_type(dfx::devices::v2::Type::UNKNOWN_DEVICE_TYPE);
    }

    // Registering a device is the only time we really don't have any type of token to provide
    ClientContext context;
    initializeClientContext(config, context, "");

    {
        ::grpc::Status status = grpcDevices->Register(&context, request, &response);
        if (!status.ok()) {
            auto dfxStatus = translateGrpcStatus(status);
            if (dfxStatus.code == CLOUD_RECORD_NOT_FOUND) {
                dfxStatus.message = "Invalid license";
            }
            return dfxStatus;
        }
    }

    config.deviceToken = response.token();
    config.deviceID = response.device_id();

    return CloudStatus(CLOUD_OK);
}

CloudStatus CloudGRPC::unregisterDevice(CloudConfig& config)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, unregisterDevice(config));

    auto grpcDevices = dfx::devices::v2::API::NewStub(getChannel(config));

    dfx::devices::v2::UnregisterResponse response;
    dfx::devices::v2::UnregisterRequest request;
    request.set_device_id(config.deviceID);

    ClientContext context;
    initializeClientContext(config, context, config.deviceToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcDevices->Unregister(&context, request, &response));

    return CloudStatus(CLOUD_OK);
}

CloudStatus CloudGRPC::validateToken(const CloudConfig& config, const std::string& userToken)
{
    DFX_CLOUD_VALIDATOR_MACRO(CloudValidator, validateToken(config, userToken));

    auto grpcAuth = dfx::auth::v1::API::NewStub(getChannel(config));

    dfx::auth::v1::ValidateTokenRequest request;
    request.set_token(userToken);
    request.set_organization_identifier(config.authOrg);
    dfx::auth::v1::ValidateTokenResponse response;

    ClientContext context;
    initializeClientContext(config, context, "");

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcAuth->ValidateToken(&context, request, &response));

    if (response.has_valid_until()) {
        time_t currentTime;
        time(&currentTime);

        auto tokenTime = google::protobuf::util::TimeUtil::TimestampToTimeT(response.valid_until());

        if (difftime(tokenTime, currentTime) > 0) {
            return CloudStatus(CLOUD_OK);
        } else {
            return CloudStatus(CLOUD_TOKEN_EXPIRED);
        }
    }

    return CloudStatus(CLOUD_INTERNAL_ERROR, "Token missing value");
}

CloudStatus CloudGRPC::switchEffectiveOrganization(CloudConfig& config, const std::string& organizationID)
{
    dfx::auth::v1::SwitchEffectiveOrganizationResponse response;
    dfx::auth::v1::SwitchEffectiveOrganizationRequest request;
    request.set_effective_organization_identifier(organizationID);

    auto grpcAuth = dfx::auth::v1::API::NewStub(getChannel(config));

    ClientContext context;
    initializeClientContext(config, context, "");

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcAuth->SwitchEffectiveOrganization(&context, request, &response));

    config.authToken = response.token();
    return CloudStatus(CLOUD_OK);
}

// *********************************************************************************
// DEVICE SECTION
// *********************************************************************************

std::shared_ptr<DeviceAPI> CloudGRPC::device(const CloudConfig& config)
{
    return std::make_shared<DeviceGRPC>(config, std::static_pointer_cast<CloudGRPC>(shared_from_this()));
}

std::shared_ptr<MeasurementAPI> CloudGRPC::measurement(const CloudConfig& config)
{
    return std::make_shared<MeasurementGRPC>(config, std::static_pointer_cast<CloudGRPC>(shared_from_this()));
}

std::shared_ptr<MeasurementStreamAPI> CloudGRPC::measurementStream(const CloudConfig& config)
{
    return std::make_shared<MeasurementStreamGRPC>(config, std::static_pointer_cast<CloudGRPC>(shared_from_this()));
}

std::shared_ptr<SignalAPI> CloudGRPC::signal(const CloudConfig& config)
{
    return std::make_shared<SignalGRPC>(config, std::static_pointer_cast<CloudGRPC>(shared_from_this()));
}

std::shared_ptr<StudyAPI> CloudGRPC::study(const CloudConfig& config)
{
    return std::make_shared<StudyGRPC>(config, std::static_pointer_cast<CloudGRPC>(shared_from_this()));
}

std::shared_ptr<UserAPI> CloudGRPC::user(const CloudConfig& config)
{
    return std::make_shared<UserGRPC>(config, std::static_pointer_cast<CloudGRPC>(shared_from_this()));
}

std::shared_ptr<::grpc::Channel> CloudGRPC::getChannel(const CloudConfig& config)
{
    std::string targetAddress = getServerURL(config.serverHost, config.serverPort);
    if (!config.secure) {
        return ::grpc::CreateChannel(targetAddress, ::grpc::InsecureChannelCredentials());
    } else {
        ::grpc::SslCredentialsOptions ssl_options;

        std::string rootCA = getRootCA(config);
        if (rootCA.length() > 0) {
            ssl_options.pem_root_certs = rootCA;
        }

        ::grpc::ChannelArguments args;
        return ::grpc::CreateCustomChannel(targetAddress, ::grpc::SslCredentials(ssl_options), args);
    }
}

CloudStatus CloudGRPC::getServerStatus(CloudConfig& config)
{
    auto channel = getChannel(config);
    if (channel) {
        return CloudStatus(CLOUD_OK);
    }
    return CloudStatus(CLOUD_TRANSPORT_FAILURE);
}
