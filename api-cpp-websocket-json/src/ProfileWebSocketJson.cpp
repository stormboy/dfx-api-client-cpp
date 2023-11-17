// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/websocket/json/ProfileWebSocketJson.hpp"
#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/web/WebServiceDetail.hpp"
#include "dfx/api/websocket/json/CloudWebSocketJson.hpp"

#include "nlohmann/json.hpp"
#include <string>

using namespace dfx::api;
using namespace dfx::api::websocket::json;

ProfileWebSocketJson::ProfileWebSocketJson(const CloudConfig& config,
                                           std::shared_ptr<CloudWebSocketJson> cloudWebSocketJson)
    : cloudWebSocketJson(std::move(cloudWebSocketJson))
{
}

CloudStatus ProfileWebSocketJson::create(const CloudConfig& config,
                                         const std::string& name,
                                         const std::string& email,
                                         Profile& profile)
{
    DFX_CLOUD_VALIDATOR_MACRO(ProfileValidator, create(config, name, email, profile));

    nlohmann::json request = {{"Name", name}, {"Email", email}};
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/profiles/create-profile
    auto result = cloudWebSocketJson->sendMessageJson(config, web::Profiles::Create, {}, {}, request, response);

    if (result.OK()) {
        auto profileID = response["ID"].get<std::string>();
        return retrieve(config, profileID, profile);
    }

    return result;
}

CloudStatus ProfileWebSocketJson::list(const CloudConfig& config,
                                       const std::unordered_map<ProfileFilter, std::string>& filters,
                                       uint16_t offset,
                                       std::vector<Profile>& profiles,
                                       int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(ProfileValidator, list(config, filters, offset, profiles, totalCount));

    totalCount = -1; // Return unknown -1, zero would be a literal zero

    std::string accountID;
    nlohmann::json query;
    for (auto& filter : filters) {
        switch (filter.first) {
            case ProfileFilter::ProfileName:
                query["UserProfileName"] = filter.second;
                break;
            case ProfileFilter::ProfileStatus:
                query["Status"] = filter.second;
                break;
            case ProfileFilter::AccountId:
                accountID = filter.second;
                break;
            default:
                return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "Invalid filter given");
        }
    }
    query["Offset"] = offset;

    nlohmann::json request;
    nlohmann::json response;
    CloudStatus result(CLOUD_OK);
    if (accountID.empty()) {
        // https://dfxapiversion10.docs.apiary.io/#reference/0/profiles/list-profiles
        result = cloudWebSocketJson->sendMessageJson(config, web::Profiles::List, {}, query, request, response);
    } else {
        nlohmann::json params;
        params["ID"] = accountID;

        // https://dfxapiversion10.docs.apiary.io/#reference/0/profiles/list-profiles-by-user
        result =
            cloudWebSocketJson->sendMessageJson(config, web::Profiles::ListByUser, params, query, request, response);
    }
    if (result.OK()) {
        if (!response.empty()) {
            std::vector<Profile> profilesTemp = response;
            profiles.insert(profiles.end(), profilesTemp.begin(), profilesTemp.end());

            if (profilesTemp.size() > 0) {
                // First element assuming there is one will have a TotalCount field which makes for
                // a non-uniform JSON schema so custom decode here
                if (response[0].contains("TotalCount")) {
                    totalCount = response[0]["TotalCount"];
                }
            }
        }
    }

    return result;
}

CloudStatus ProfileWebSocketJson::retrieve(const CloudConfig& config, const std::string& profileID, Profile& profile)
{
    DFX_CLOUD_VALIDATOR_MACRO(ProfileValidator, retrieve(config, profileID, profile));

    nlohmann::json request;
    nlohmann::json response;

    nlohmann::json params;
    params["ID"] = profileID;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/profiles/retrieve-profile
    auto result = cloudWebSocketJson->sendMessageJson(config, web::Profiles::Retrieve, params, {}, request, response);

    if (result.OK()) {
        if (!response.is_array()) {
            profile = response; // Must be a map of one value
            profile.id = profileID;
        } else {
            // Try looking through the list of returned items by name and email. This part of the
            // spec does not seem to be fully documented. I had planned on using list() to obtain
            // but server gave me back a list when querying by empty ID so just rolling with it.
            bool notFound = true;
            for (auto record : response) {
                std::string email = record["Email"];
                std::string name = record["Name"];
                if ((email.compare(profile.email) == 0) && (name.compare(profile.name) == 0)) {
                    profile = record;
                    profile.id = profileID;
                    notFound = false;
                    break;
                }
            }
            if (notFound) {
                return CloudStatus(CLOUD_RECORD_NOT_FOUND);
            }
        }
    }

    return result;
}

CloudStatus ProfileWebSocketJson::update(const CloudConfig& config, const Profile& profile)
{
    DFX_CLOUD_VALIDATOR_MACRO(ProfileValidator, update(config, profile));

    nlohmann::json request = {
        {"Name", profile.name}, {"Email", profile.email}, {"Status", ProfileStatusMapper::getString(profile.status)}};
    nlohmann::json response;

    nlohmann::json params;
    params["ID"] = profile.id;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/profiles/update-profile
    return cloudWebSocketJson->sendMessageJson(config, web::Profiles::Update, params, {}, request, response);
}

CloudStatus ProfileWebSocketJson::remove(const CloudConfig& config, const std::string& profileID)
{
    DFX_CLOUD_VALIDATOR_MACRO(ProfileValidator, remove(config, profileID));

    nlohmann::json request;
    nlohmann::json response;

    nlohmann::json params;
    params["ID"] = profileID;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/profiles/remove-profile
    return cloudWebSocketJson->sendMessageJson(config, web::Profiles::Remove, params, {}, request, response);
}