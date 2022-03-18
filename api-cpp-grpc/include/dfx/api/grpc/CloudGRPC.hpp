// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_GRPC_API_H
#define DFX_API_CLOUD_GRPC_API_H

#include "dfx/api/CloudAPI.hpp"
#include "dfx/api/CloudConfig.hpp"

#include <grpcpp/grpcpp.h>
#include <list>
#include <memory>
#include <string>

namespace dfx::api::grpc
{

/**
 * @class CloudGrpcAPI CloudGrpcAPI.h "dfx/api/gprc/CloudGrpcAPI.hpp"
 *
 * @brief CloudGrpcAPI provides a C++ class which communicates with the Nuralogix
 *        DFX API version 3 servers using gRPC communications.
 */
class CloudGRPC : public CloudAPI
{
public:
    explicit CloudGRPC(const CloudConfig& config);

    // Technically, we will exit with memory linked to the protobuf library which could be
    // relinquished by google::protobuf::ShutdownProtobufLibrary() but that is a one-shot
    // deal which makes it useless to call here (even protected by mutex) as our object
    // might come and go independent of the process unloading the library.
    // Dumb protobuf design.
    ~CloudGRPC() override = default;

    CloudStatus connect(const CloudConfig& config) override;

    CloudStatus getServerStatus(CloudConfig& config) override;

    // *********************************************************************************
    // AUTHENTICATION SECTION
    // *********************************************************************************

    CloudStatus login(CloudConfig& config) override;

    CloudStatus logout(CloudConfig& config) override;

    CloudStatus registerDevice(CloudConfig& config, const std::string& appName, const std::string& appVersion) override;

    CloudStatus unregisterDevice(CloudConfig& config) override;

    CloudStatus validateToken(const CloudConfig& config, const std::string& userToken) override;

    CloudStatus switchEffectiveOrganization(CloudConfig& config, const std::string& organizationID) override;

    // *********************************************************************************
    // SERVICE SECTION
    // *********************************************************************************
    std::shared_ptr<DeviceAPI> device(const CloudConfig& config) override;

    std::shared_ptr<MeasurementAPI> measurement(const CloudConfig& config) override;

    std::shared_ptr<MeasurementStreamAPI> measurementStream(const CloudConfig& config) override;

    std::shared_ptr<SignalAPI> signal(const CloudConfig& config) override;

    std::shared_ptr<StudyAPI> study(const CloudConfig& config) override;

    std::shared_ptr<UserAPI> user(const CloudConfig& config) override;

private:
    friend class DeviceGRPC;

    friend class MeasurementGRPC;

    friend class MeasurementStreamGRPC;

    friend class OrganizationGRPC;

    friend class SignalGRPC;

    friend class StudyGRPC;

    friend class UserGRPC;

    static void initializeClientContext(const CloudConfig& config,
                                        ::grpc::ClientContext& context,
                                        const std::string& token);

    static void setDeadlineClientContext(const CloudConfig& config, ::grpc::ClientContext& context);
    static void setAuthTokenClientContext(const CloudConfig& config,
                                          ::grpc::ClientContext& context,
                                          const std::string& token);

    static std::string getServerURL(const std::string& hostname, int port);

    static std::shared_ptr<::grpc::Channel> getChannel(const CloudConfig& config);

    static CloudStatus translateGrpcStatus(const ::grpc::Status& status);
};

} // namespace dfx::api::grpc

#endif // DFX_API_CLOUD_GRPC_API_H
