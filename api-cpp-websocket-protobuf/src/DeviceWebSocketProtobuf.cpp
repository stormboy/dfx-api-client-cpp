// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/websocket/protobuf/DeviceWebSocketProtobuf.hpp"

#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/websocket/protobuf/CloudWebSocketProtobuf.hpp"

#include "dfx/proto/devices.pb.h"

#include "fmt/format.h"

using namespace dfx::api;
using namespace dfx::api::websocket::protobuf;

DeviceWebSocketProtobuf::DeviceWebSocketProtobuf(const CloudConfig& config,
                                                 std::shared_ptr<CloudWebSocketProtobuf> cloudWebSocketProtobuf)
    : cloudWebSocketProtobuf(std::move(cloudWebSocketProtobuf))
{
}

CloudStatus DeviceWebSocketProtobuf::create(const CloudConfig& config,
                                            const std::string& name,
                                            DeviceType type,
                                            const std::string& identifier,
                                            const std::string& version,
                                            Device& device)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, create(config, name, type, identifier, version, device));

    dfx::proto::devices::CreateRequest request;
    dfx::proto::devices::CreateResponse response;

    request.set_name(name);
    request.set_devicetypeid(DeviceTypeMapper::toString.at(type));
    auto deviceType = CloudAPI::getPlatform();
    deviceType = "LINUX";
    request.set_platform(deviceType);
    request.set_identifier(identifier);
    request.set_version(version);

    auto status = cloudWebSocketProtobuf->sendMessage(dfx::api::web::Devices::Create, request, response);
    if (status.OK()) {
        const auto& id = response.id();
        return retrieve(config, id, device);
    }
    return status;
}

CloudStatus DeviceWebSocketProtobuf::list(const CloudConfig& config,
                                          const std::unordered_map<DeviceFilter, std::string>& filters,
                                          uint16_t offset,
                                          std::vector<Device>& deviceList,
                                          int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, list(config, filters, offset, deviceList, totalCount));

    // Protobuf doesn't return a total count
    totalCount = -1;

    dfx::proto::devices::ListRequest request;
    dfx::proto::devices::ListResponse response;

    // REST: https://dfxapiversion10.docs.apiary.io/#reference/0/devices/list
    // WebSocket: Only has ID, but that's essentially a retrieve?
    for (auto& filter : filters) {
        switch (filter.first) {
            case DeviceFilter::ID:
                request.mutable_query()->set_id(filter.second);
                break;
            default:
                return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "Unexpected list filter key");
        }
    }

    auto status = cloudWebSocketProtobuf->sendMessage(dfx::api::web::Devices::List, request, response);
    if (status.OK()) {
        for (int index = 0; index < response.values_size(); index++) {
            const auto& pbDevice = response.values(index);
            Device device;
            device.id = pbDevice.id();
            device.name = pbDevice.name();
            device.type = DeviceTypeMapper::getEnum(pbDevice.devicetypeid());
            device.status = DeviceStatusMapper::getEnum(pbDevice.statusid());
            device.identifier = pbDevice.identifier();
            device.version = pbDevice.version();
            device.numberMeasurements = pbDevice.measurements();
            device.createdEpochSeconds = pbDevice.created();
            device.updatedEpochSeconds = pbDevice.updated();

            deviceList.push_back(device);
        }
    }

    return status;
}

CloudStatus DeviceWebSocketProtobuf::retrieve(const CloudConfig& config, const std::string& deviceID, Device& device)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, retrieve(config, deviceID, device));

    dfx::proto::devices::RetrieveRequest request;
    dfx::proto::devices::RetrieveResponse response;

    request.mutable_params()->set_id(deviceID);

    auto status = cloudWebSocketProtobuf->sendMessage(dfx::api::web::Devices::Retrieve, request, response);
    if (status.OK()) {
        device.id = response.id();
        device.name = response.name();
        device.type = DeviceTypeMapper::getEnum(response.devicetypeid());
        device.status = DeviceStatusMapper::getEnum(response.statusid());
        device.identifier = response.identifier();
        device.version = response.version();
        device.numberMeasurements = response.measurements();
        device.createdEpochSeconds = response.created();
        device.updatedEpochSeconds = response.updated();
    }
    return status;
}

CloudStatus DeviceWebSocketProtobuf::update(const CloudConfig& config, const Device& device)
{
    DFX_CLOUD_VALIDATOR_MACRO(DeviceValidator, update(config, device));

    dfx::proto::devices::UpdateRequest request;
    dfx::proto::devices::UpdateResponse response;

    request.mutable_params()->set_id(device.id);
    request.set_name(device.name);
    request.set_devicetypeid(DeviceTypeMapper::getString(device.type));
    request.set_statusid(DeviceStatusMapper::getString(device.status));
    request.set_identifier(device.identifier);
    request.set_version(device.version);

    // Server returns empty response
    return cloudWebSocketProtobuf->sendMessage(dfx::api::web::Devices::Update, request, response);
}

CloudStatus DeviceWebSocketProtobuf::remove(const CloudConfig& config, const std::string& deviceID)
{
    // No longer supported but to avoid runtime breaking clients, just return OK
    return CloudStatus(CLOUD_OK);
}