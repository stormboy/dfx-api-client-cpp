// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/websocket/json/UserWebSocketJson.hpp"
#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/websocket/json/CloudWebSocketJson.hpp"

#include "fmt/format.h"

using namespace dfx::api;
using namespace dfx::api::websocket::json;

UserWebSocketJson::UserWebSocketJson(const CloudConfig& config, std::shared_ptr<CloudWebSocketJson> cloudWebSocketJson)
    : cloudWebSocketJson(std::move(cloudWebSocketJson))
{
}

CloudStatus UserWebSocketJson::create(const CloudConfig& config,
                                      const std::string& email,
                                      const std::string& password,
                                      const std::string& _role,
                                      const std::string& firstName,
                                      const std::string& lastName,
                                      const std::string& phoneNumber,
                                      const std::string& gender,
                                      const std::string& dateOfBirth,
                                      const uint16_t heightCM,
                                      const uint16_t weightKG,
                                      std::string& userID)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator,
                              create(config,
                                     email,
                                     password,
                                     _role,
                                     firstName,
                                     lastName,
                                     phoneNumber,
                                     gender,
                                     dateOfBirth,
                                     heightCM,
                                     weightKG,
                                     userID));

    nlohmann::json request;
    nlohmann::json response;

    request["FirstName"] = firstName;
    request["LastName"] = lastName;
    request["Email"] = email;
    request["Password"] = password;
    request["PhoneNumber"] = phoneNumber;
    request["Gender"] = gender;
    request["DateOfBirth"] = dateOfBirth;
    request["HeightCm"] = heightCM;
    request["WeightKg"] = weightKG;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/users/create
    auto result = cloudWebSocketJson->sendMessageJson(config, web::Users::Create, {}, {}, request, response);

    if (result.OK()) {
        userID = response["ID"];
    }

    return result;
}

CloudStatus UserWebSocketJson::list(const CloudConfig& config,
                                    const std::unordered_map<UserFilter, std::string>& filters,
                                    uint16_t offset,
                                    std::vector<User>& users,
                                    int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, list(config, filters, offset, users, totalCount));

    nlohmann::json request;
    nlohmann::json response;

    totalCount = -1;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/organizations/users
    auto result = cloudWebSocketJson->sendMessageJson(config, web::Organizations::Users,
                                                      {}, {}, request, response);

    if (result.OK()) {
        // Iterate over the JSON array and construct User objects
        for (const auto& user : response) {
            User u;
            from_json(user, u);
            users.push_back(u);

            if (user.contains("TotalCount")) {
                totalCount = user["TotalCount"];
            }
        }
    }

    return result;
}

CloudStatus UserWebSocketJson::retrieve(const CloudConfig& config, User& user)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, retrieve(config, user));

    nlohmann::json request;
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/users/retrieve
    auto result = cloudWebSocketJson->sendMessageJson(config, web::Users::Retrieve,
                                                      {}, {}, request, response);

    if ( result.OK() ) {
        from_json(response, user);
    }

    return result;
}

CloudStatus UserWebSocketJson::update(const CloudConfig& config, const User& user)
{
    DFX_CLOUD_VALIDATOR_MACRO(UserValidator, update(config, user));

    nlohmann::json request;
    nlohmann::json response;

    request = user;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/users/update
    auto result = cloudWebSocketJson->sendMessageJson(config, web::Users::Update,
                                                      {}, {}, request, response);
    return result;
}
