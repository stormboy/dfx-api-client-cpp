// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/validator/SignalValidator.hpp"

#include "CloudValidatorMacros.hpp"

using namespace dfx::api;
using namespace dfx::api::validator;

const SignalValidator& SignalValidator::instance()
{
    static const SignalValidator instance;
    return instance;
}

CloudStatus SignalValidator::list(const CloudConfig& config,
                                  const std::unordered_map<SignalFilter, std::string>& filters,
                                  uint16_t offset,
                                  std::vector<Signal>& signals,
                                  int16_t& totalCount)
{
    return CloudStatus(CLOUD_OK);
}

CloudStatus SignalValidator::retrieve(const CloudConfig& config, const std::string& signalID, Signal& signal)
{
    MACRO_RETURN_ERROR_IF_EMPTY(signal.id);
    return CloudStatus(CLOUD_OK);
}

CloudStatus SignalValidator::retrieveMultiple(const CloudConfig& config,
                                              const std::vector<std::string>& signalIDs,
                                              std::vector<Signal>& signals)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    return CloudStatus(CLOUD_OK);
}

CloudStatus SignalValidator::retrieveStudySignalIDs(const CloudConfig& config,
                                                    const std::string& studyID,
                                                    std::vector<std::string>& signalIDs)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(studyID);
    return CloudStatus(CLOUD_OK);
}

CloudStatus SignalValidator::retrieveSignalDetail(const CloudConfig& config,
                                                  const std::string& signalID,
                                                  Signal& signalDetail)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(signalID);
    return CloudStatus(CLOUD_OK);
}

CloudStatus SignalValidator::retrieveSignalDetails(const CloudConfig& config,
                                                   const std::list<std::string>& signalIDs,
                                                   std::vector<Signal>& signalDetails)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(signalIDs);
    return CloudStatus(CLOUD_OK);
}