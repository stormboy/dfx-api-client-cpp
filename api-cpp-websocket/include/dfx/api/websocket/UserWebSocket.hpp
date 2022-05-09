// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_USER_WEBSOCKET_H
#define DFX_API_CLOUD_USER_WEBSOCKET_H

#include "dfx/api/UserAPI.hpp"

#include <memory>

namespace dfx::api::websocket
{

class CloudWebSocket;

class UserWebSocket : public UserAPI
{
public:
    UserWebSocket(const CloudConfig& config, std::shared_ptr<CloudWebSocket> cloudWebSocket);

    ~UserWebSocket() override = default;

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

    CloudStatus
    retrieve(const CloudConfig& config, const std::string& userID, const std::string& email, User& user) override;

    CloudStatus
    update(const CloudConfig& config, const std::string& userID, const std::string& email, const User& user) override;

    CloudStatus remove(const CloudConfig& config, const std::string& email, const std::string& userID) override;

private:
    std::shared_ptr<CloudWebSocket> cloudWebSocket;
};

} // namespace dfx::api::websocket

#endif // DFX_API_CLOUD_USER_WEBSOCKET_H
