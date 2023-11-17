// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/rest/SignalREST.hpp"
#include "dfx/api/rest/CloudREST.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

#include "nlohmann/json.hpp"
#include <fmt/format.h>
#include <sstream>
#include <string>

using namespace dfx::api;
using namespace dfx::api::rest;

CloudStatus SignalREST::list(const CloudConfig& config,
                             const std::unordered_map<SignalFilter, std::string>& filters,
                             uint16_t offset,
                             std::vector<Signal>& signals,
                             int16_t& totalCount)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "REST", "signal list"));
}

CloudStatus SignalREST::retrieve(const CloudConfig& config, const std::string& signalID, Signal& signal)
{
    DFX_CLOUD_VALIDATOR_MACRO(SignalValidator, retrieve(config, signalID, signal));

    nlohmann::json request;
    nlohmann::json response;
    auto status =
        CloudREST::performRESTCall(config, web::Signals::Retrieve, config.authToken, {signalID}, request, response);
    if (status.OK()) {
        signal = response;
        signal.id = signalID;
    }
    return status;
}

CloudStatus SignalREST::retrieveMultiple(const CloudConfig& config,
                                         const std::vector<std::string>& signalIDs,
                                         std::vector<Signal>& signals)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "REST", "signal retrieveMultiple"));
}

CloudStatus SignalREST::retrieveStudySignalIDs(const CloudConfig& config,
                                               const std::string& studyID,
                                               std::vector<std::string>& signalIDs)
{
    DFX_CLOUD_VALIDATOR_MACRO(SignalValidator, retrieveStudySignalIDs(config, studyID, signalIDs));

    nlohmann::json request;
    nlohmann::json response;
    auto status =
        CloudREST::performRESTCall(config, web::Studies::Retrieve, config.authToken, {studyID}, request, response);
    if (status.OK()) {
        for (int i = 0; i < response["Signals"].size(); i++) {
            auto strSig = response["Signals"][i]["ID"].get<std::string>();
            signalIDs.push_back(strSig);
        }
    }

    return status;
}

CloudStatus SignalREST::retrieveSignalDetail(const CloudConfig& config,
                                             const std::string& signalID,
                                             Signal& signalDetail)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "REST", "signal retrieveSignalDetail"));
}

CloudStatus SignalREST::retrieveSignalDetails(const CloudConfig& config,
                                              const std::list<std::string>& signalIDs,
                                              std::vector<Signal>& signalDetails)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "REST", "signal retrieveSignalDetails"));
}
