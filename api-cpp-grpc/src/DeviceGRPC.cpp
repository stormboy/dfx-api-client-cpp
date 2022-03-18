// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/grpc/DeviceGRPC.hpp"
#include "dfx/api/grpc/CloudGRPC.hpp"
#include "dfx/api/types/FilterHelpers.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

#include <fmt/format.h>

#include "CloudGRPCMacros.hpp"

using dfx::api::CloudAPI;
using dfx::api::CloudConfig;
using dfx::api::CloudStatus;
using dfx::api::Device;
using dfx::api::DeviceAPI;
using dfx::api::DeviceType;

using namespace dfx::api::grpc;
using namespace ::grpc;

DeviceGRPC::DeviceGRPC(const CloudConfig& config, const std::shared_ptr<CloudGRPC>& cloudGRPC)
{
    grpcDeviceStub = dfx::devices::v2::API::NewStub(cloudGRPC->getChannel(config));
}

CloudStatus create(const CloudConfig& config,
                   const std::string& name,
                   const DeviceType type,
                   const std::string& identifier,
                   const std::string& version,
                   Device& device)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE);
}

CloudStatus DeviceGRPC::list(const CloudConfig& config,
                             const std::unordered_map<DeviceFilter, std::string>& filters,
                             uint16_t offset,
                             std::vector<Device>& devices,
                             int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, list(config, filters, offset, devices, totalCount));

    if (filters.size() > 0) {
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "Unexpected list filter key");
    }

    dfx::devices::v2::ListResponse response;
    dfx::devices::v2::ListRequest request;
    request.set_limit(config.listLimit);
    request.set_offset(offset);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcDeviceStub->List(&context, request, &response));
    totalCount = response.total();

    const auto numberDevices = response.devices_size();
    if (numberDevices == 0) {
        // It is possible with offset/limit to have no devices - but calling retrieveMultiple with an
        // empty set would return error so short-circuit here.
        return CloudStatus(CLOUD_OK);
    }

    bool fullObject = getFilterBool(filters, DeviceFilter::FullObject, false);
    if (fullObject) {
        std::vector<std::string> deviceIDs;
        for (auto index = 0; index < numberDevices; index++) {
            const auto& deviceData = response.mutable_devices(index);
            deviceIDs.push_back(deviceData->id());
        }
        return retrieveMultiple(config, deviceIDs, devices);
    } else {
        for (auto index = 0; index < numberDevices; index++) {
            const auto& deviceData = response.mutable_devices(index);
            Device device;
            device.id = deviceData->id();
            device.name = deviceData->name();
            devices.push_back(device);
        }
        return CloudStatus(CLOUD_OK);
    }
}

CloudStatus DeviceGRPC::retrieve(const CloudConfig& config, const std::string& deviceID, Device& deviceInfo)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, retrieve(config, deviceID, deviceInfo));

    dfx::devices::v2::RetrieveResponse response;
    dfx::devices::v2::RetrieveRequest request;
    request.set_id(deviceID);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcDeviceStub->Retrieve(&context, request, &response));

    if (response.has_device()) {
        const auto& device = response.device();

        deviceInfo.id = device.id();
        deviceInfo.name = device.name();
        deviceInfo.type = static_cast<DeviceType>(device.device_type());
        deviceInfo.status = static_cast<DeviceStatus>(device.status());
        deviceInfo.identifier = device.identifier();
        deviceInfo.version = device.version();
        deviceInfo.createdEpochSeconds = device.created().seconds();
        deviceInfo.updatedEpochSeconds = device.updated().seconds();
        deviceInfo.numberMeasurements = 0; // Undefined on gRPC
    } else {
        return CloudStatus(CLOUD_RECORD_NOT_FOUND, fmt::format("Device id {} not found", deviceID));
    }

    return CloudStatus(CLOUD_OK);
}

CloudStatus DeviceGRPC::retrieveMultiple(const CloudConfig& config,
                                         const std::vector<string>& deviceIDs,
                                         std::vector<Device>& devices)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, retrieveMultiple(config, deviceIDs, devices));

    dfx::devices::v2::RetrieveMultipleResponse response;
    dfx::devices::v2::RetrieveMultipleRequest request;
    for (const auto& id : deviceIDs) {
        request.add_ids(id);
    }

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcDeviceStub->RetrieveMultiple(&context, request, &response));

    const auto numberDevices = response.devices_size();
    for (size_t index = 0; index < numberDevices; index++) {
        const auto& deviceData = response.devices(static_cast<int>(index));

        Device device;
        device.id = deviceData.id();
        device.name = deviceData.name();
        device.type = static_cast<DeviceType>(deviceData.device_type());
        device.status = static_cast<DeviceStatus>(deviceData.status());
        device.identifier = deviceData.identifier();
        device.version = deviceData.version();
        device.createdEpochSeconds = deviceData.created().seconds();
        device.updatedEpochSeconds = deviceData.updated().seconds();
        devices.push_back(device);
    }

    return CloudStatus(CLOUD_OK);
}

CloudStatus DeviceGRPC::update(const CloudConfig& config, const Device& device)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, update(config, device));

    dfx::devices::v2::UpdateResponse response;
    dfx::devices::v2::UpdateRequest request;
    request.set_id(device.id);
    request.set_name(device.name);
    request.set_identifier(device.identifier);
    request.set_device_type(static_cast<::dfx::devices::v2::Type>(device.type));
    request.set_status(static_cast<::dfx::devices::v2::Status>(device.status));
    request.set_version(device.version);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcDeviceStub->Update(&context, request, &response));

    return CloudStatus(CLOUD_OK);
}

CloudStatus DeviceGRPC::remove(const CloudConfig& config, const std::string& deviceID)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, remove(config, deviceID));

    dfx::devices::v2::RemoveResponse response;
    dfx::devices::v2::RemoveRequest request;
    request.set_id(deviceID);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcDeviceStub->Remove(&context, request, &response));

    return CloudStatus(CLOUD_OK);
}
