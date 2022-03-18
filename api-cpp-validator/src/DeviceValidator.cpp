// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/validator/DeviceValidator.hpp"

#include "CloudValidatorMacros.hpp"

using namespace dfx::api;
using namespace dfx::api::validator;

const DeviceValidator& DeviceValidator::instance()
{
    static const DeviceValidator instance;
    return instance;
}

CloudStatus DeviceValidator::create(const CloudConfig& config,
                                    const std::string& name,
                                    const DeviceType /*type*/,
                                    const std::string& identifier,
                                    const std::string& version,
                                    Device& /*device*/)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(name);
    MACRO_RETURN_ERROR_IF_EMPTY(identifier);
    MACRO_RETURN_ERROR_IF_EMPTY(version);
    return CloudStatus(CLOUD_OK);
}

CloudStatus DeviceValidator::list(const CloudConfig& config,
                                  const std::unordered_map<DeviceFilter, std::string>& /*filters*/,
                                  uint16_t offset,
                                  std::vector<Device>& /*deviceList*/,
                                  int16_t& /*totalCount*/)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    return CloudStatus(CLOUD_OK);
}

CloudStatus DeviceValidator::retrieve(const CloudConfig& config, const std::string& deviceID, Device& deviceInfo)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(deviceID);
    return CloudStatus(CLOUD_OK);
}

CloudStatus DeviceValidator::retrieveMultiple(const CloudConfig& config,
                                              const std::vector<std::string>& deviceIDs,
                                              std::vector<Device>& devices)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(deviceIDs);
    return CloudStatus(CLOUD_OK);
}

CloudStatus DeviceValidator::update(const CloudConfig& config, const Device& device)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    return CloudStatus(CLOUD_OK);
}

CloudStatus DeviceValidator::remove(const CloudConfig& config, const std::string& deviceID)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(deviceID);
    return CloudStatus(CLOUD_OK);
}
