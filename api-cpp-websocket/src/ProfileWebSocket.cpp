// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/websocket/ProfileWebSocket.hpp"
#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/web/WebServiceDetail.hpp"
#include "dfx/api/websocket/CloudWebSocket.hpp"

#include "dfx/proto/profiles.pb.h"

#include "nlohmann/json.hpp"
#include <fmt/format.h>
#include <string>

using namespace dfx::api;
using namespace dfx::api::websocket;
using nlohmann::json;

ProfileWebSocket::ProfileWebSocket(const CloudConfig& config, std::shared_ptr<CloudWebSocket> cloudWebSocket)
    : cloudWebSocket(std::move(cloudWebSocket))
{
}

CloudStatus
ProfileWebSocket::create(const CloudConfig& config, const std::string& name, const std::string& email, Profile& profile)
{
    dfx::proto::profiles::CreateRequest request;
    dfx::proto::profiles::CreateResponse response;

    request.set_name(name);
    request.set_email(email);

    auto status = cloudWebSocket->sendMessage(dfx::api::web::Profiles::Create, request, response);
    if (status.OK()) {
        auto profileID = response.id();
        return retrieve(config, profileID, profile);
    }
    return status;
}

CloudStatus ProfileWebSocket::list(const CloudConfig& config,
                                   const std::unordered_map<ProfileFilter, std::string>& filters,
                                   uint16_t offset,
                                   std::vector<Profile>& profiles,
                                   int16_t& totalCount)
{
    // Static typing is not as nice as REST, need to break both into their own implementation here
    if (filters.find(ProfileFilter::AccountId) ==
        filters.end()) { // If user didn't provide AccountID, use Profiles::List
        dfx::proto::profiles::ListRequest request;
        dfx::proto::profiles::ListResponse response;

        std::string accountID;
        auto query = request.mutable_query();
        for (auto& filter : filters) {
            switch (filter.first) {
                case ProfileFilter::ProfileName:
                    query->set_userprofilename(filter.second);
                    break;
                case ProfileFilter::UserEmail: // Inconsistent: An option on protobuf, but not mentioned on REST API
                    query->set_useremail(filter.second);
                    break;
                case ProfileFilter::ProfileStatus:
                    return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR,
                                       "ProfileStatus filter not supported on WebSocket");
                default:
                    return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "Invalid filter given");
            }
        }

        query->set_limit(config.listLimit);
        query->set_offset(offset);

        auto status = cloudWebSocket->sendMessage(dfx::api::web::Profiles::List, request, response);
        if (status.OK()) {
            if (response.values_size() > 0) {
                totalCount = response.values_size();
                for (int i = 0; i < totalCount; i++) {
                    auto resProfile = response.mutable_values(i);
                    Profile profile;
                    profile.id = resProfile->id();
                    // profile.ownerUserEmail = resProfile->user
                    profile.name = resProfile->name();
                    profile.email = resProfile->email();
                    profile.status = ProfileStatusMapper::getEnum(resProfile->status());
                    // profile.measurementCount = resProfile->();
                    profile.createdEpochSeconds = resProfile->created();
                    profile.updatedEpochSeconds = resProfile->updated();
                    profiles.push_back(profile);
                }
            }
        }
        return status;
    } else {
        // User provided AccountID so use ListByUser
        dfx::proto::profiles::ListByUserRequest request;
        dfx::proto::profiles::ListByUserResponse response;

        std::string accountID;
        auto query = request.mutable_query();
        for (auto& filter : filters) {
            switch (filter.first) {
                case ProfileFilter::ProfileName:
                    query->set_userprofilename(filter.second);
                    break;
                case ProfileFilter::ProfileStatus:
                    return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "ProfileStatus not supported on WebSocket");
                case ProfileFilter::AccountId:
                    accountID = filter.second;
                    break;
                default:
                    return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "Invalid filter given");
            }
        }

        auto params = request.mutable_params();
        params->set_id(accountID);

        query->set_limit(config.listLimit);
        query->set_offset(offset);

        auto status = cloudWebSocket->sendMessage(dfx::api::web::Profiles::ListByUser, request, response);
        if (response.values_size() > 0) {
            totalCount = response.values_size();
            for (int i = 0; i < totalCount; i++) {
                auto resProfile = response.mutable_values(i);
                Profile profile;
                profile.id = resProfile->id();
                profile.name = resProfile->name();
                profile.email = resProfile->email();
                profile.status = ProfileStatusMapper::getEnum(resProfile->status());
                profile.createdEpochSeconds = resProfile->created();
                profile.updatedEpochSeconds = resProfile->updated();
                profiles.push_back(profile);
            }
        }
        return status;
    }
}

CloudStatus ProfileWebSocket::retrieve(const CloudConfig& config, const std::string& profileID, Profile& profile)
{
    DFX_CLOUD_VALIDATOR_MACRO(ProfileValidator, retrieve(config, profileID, profile));

    dfx::proto::profiles::RetrieveRequest request;
    dfx::proto::profiles::RetrieveResponse response;

    auto params = request.mutable_params();
    params->set_id(profileID);

    auto status = cloudWebSocket->sendMessage(dfx::api::web::Profiles::Retrieve, request, response);
    if (status.OK()) {
        profile.id = profileID;
        profile.name = response.name();
        profile.email = response.email();
        profile.status = ProfileStatusMapper::getEnum(response.status());
        profile.createdEpochSeconds = response.created();
        profile.updatedEpochSeconds = response.updated();
    }
    return status;
}

CloudStatus ProfileWebSocket::update(const CloudConfig& config, const Profile& profile)
{
    dfx::proto::profiles::UpdateRequest request;
    dfx::proto::profiles::UpdateResponse response;

    auto params = request.mutable_params();
    params->set_id(profile.id);
    request.set_email(profile.email);
    request.set_name(profile.name);
    request.set_status(ProfileStatusMapper::getString(profile.status));

    auto Status = cloudWebSocket->sendMessage(dfx::api::web::Profiles::Update, request, response);
    return Status;
}

CloudStatus ProfileWebSocket::remove(const CloudConfig& config, const std::string& profileID)
{
    dfx::proto::profiles::RemoveRequest request;
    dfx::proto::profiles::RemoveResponse response;

    auto params = request.mutable_params();
    params->set_id(profileID);

    auto Status = cloudWebSocket->sendMessage(dfx::api::web::Profiles::Remove, request, response);
    return Status;
}