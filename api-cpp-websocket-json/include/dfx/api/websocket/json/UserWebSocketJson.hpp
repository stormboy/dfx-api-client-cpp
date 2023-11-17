// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_USER_WEBSOCKET_JSON_H
#define DFX_API_CLOUD_USER_WEBSOCKET_JSON_H

#include "dfx/api/UserAPI.hpp"

#include <memory>

namespace dfx::api::websocket::json
{

class CloudWebSocketJson;

class UserWebSocketJson : public UserAPI
{
public:
    UserWebSocketJson(const CloudConfig& config, std::shared_ptr<CloudWebSocketJson> cloudWebSocketJson);

    ~UserWebSocketJson() override = default;

    CloudStatus create(const CloudConfig& config,
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
                       std::string& userID) override;

    CloudStatus list(const CloudConfig& config,
                     const std::unordered_map<UserFilter, std::string>& filters,
                     uint16_t offset,
                     std::vector<User>& users,
                     int16_t& totalCount) override;

    CloudStatus retrieve(const CloudConfig& config, User& user) override;

    CloudStatus update(const CloudConfig& config, const User& user) override;

private:
    std::shared_ptr<CloudWebSocketJson> cloudWebSocketJson;
};

} // namespace dfx::api::websocket::json

#endif // DFX_API_CLOUD_USER_WEBSOCKET_JSON_H
