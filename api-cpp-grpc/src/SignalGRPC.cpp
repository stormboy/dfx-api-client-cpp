// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/SignalAPI.hpp"

#include "dfx/api/grpc/CloudGRPC.hpp"
#include "dfx/api/grpc/SignalGRPC.hpp"
#include "dfx/api/types/FilterHelpers.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

#include "CloudGRPCMacros.hpp"

using dfx::api::CloudAPI;
using dfx::api::CloudConfig;
using dfx::api::CloudStatus;
using dfx::api::Signal;
using dfx::api::SignalAPI;

using namespace dfx::api::grpc;
using namespace ::grpc;

SignalGRPC::SignalGRPC(const CloudConfig& config, const std::shared_ptr<CloudGRPC>& cloudGRPC)
{
    grpcStudySignalsStub = dfx::studysignals::v2::API::NewStub(cloudGRPC->getChannel(config));
    grpcSignalsStub = dfx::signals::v2::API::NewStub(cloudGRPC->getChannel(config));
}

CloudStatus SignalGRPC::list(const CloudConfig& config,
                             const std::unordered_map<SignalFilter, std::string>& filters,
                             uint16_t offset,
                             std::vector<Signal>& signals,
                             int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(SignalValidator, list(config, filters, offset, signals, totalCount));

    dfx::signals::v2::ListResponse response;
    dfx::signals::v2::ListRequest request;

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcSignalsStub->List(&context, request, &response));
    totalCount = response.total();

    const auto numberSignals = response.signals_size();
    if (numberSignals == 0) {
        // It is possible with offset/limit to have no signals
        return CloudStatus(CLOUD_OK);
    }

    bool fullObject = getFilterBool(filters, SignalFilter::FullObject, false);
    if (fullObject) {
        std::vector<std::string> signalIDs;
        for (auto index = 0; index < numberSignals; index++) {
            const auto& signalData = response.mutable_signals(index);
            signalIDs.push_back(signalData->id());
        }
        return retrieveMultiple(config, signalIDs, signals);
    } else {
        for (auto index = 0; index < numberSignals; index++) {
            const auto& signalData = response.mutable_signals(index);
            Signal signal{};
            signal.id = signalData->id();
            signal.name = signalData->name();
            signals.push_back(signal);
        }
        return CloudStatus(CLOUD_OK);
    }
}

CloudStatus SignalGRPC::retrieve(const CloudConfig& config, const std::string& signalID, Signal& signal)
{
    DFX_CLOUD_VALIDATOR_MACRO(SignalValidator, retrieve(config, signalID, signal));

    dfx::signals::v2::RetrieveResponse response;
    dfx::signals::v2::RetrieveRequest request;

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcSignalsStub->Retrieve(&context, request, &response));

    if (response.has_signal()) {
        const auto& obj = response.signal();

        signal.id = signalID;
        signal.name = obj.name();
        signal.description = obj.description();
        signal.initialDelaySeconds = obj.initial_delay_sec();
        signal.modelMinSeconds = obj.model_min_sec();
        signal.unit = obj.unit();
        signal.modelMinAmplitude = obj.model_min_amplitude();
        signal.modelMaxAmplitude = obj.model_max_amplitude();
        signal.humanMinAmplitude = obj.human_min_amplitude();
        signal.humanMaxAmplitude = obj.human_max_amplitude();
    }

    return CloudStatus(CLOUD_OK);
}

CloudStatus SignalGRPC::retrieveMultiple(const CloudConfig& config,
                                         const std::vector<std::string>& signalIDs,
                                         std::vector<Signal>& signals)
{
    DFX_CLOUD_VALIDATOR_MACRO(SignalValidator, retrieveMultiple(config, signalIDs, signals));

    dfx::signals::v2::RetrieveMultipleResponse response;
    dfx::signals::v2::RetrieveMultipleRequest request;

    for (auto& id : signalIDs) {
        request.add_ids(id);
    }

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcSignalsStub->RetrieveMultiple(&context, request, &response));

    const auto numberSignals = response.signals_size();

    // Validate will occur by first retrieve call
    std::vector<Signal> signalList;
    signalList.reserve(numberSignals);
    for (const auto& [key, value] : response.signals()) {
        Signal signal{};
        signal.id = key;
        signal.name = value.name();
        signal.description = value.description();
        signal.initialDelaySeconds = value.initial_delay_sec();
        signal.modelMinSeconds = value.model_min_sec();
        signal.unit = value.unit();
        signal.modelMinAmplitude = value.model_min_amplitude();
        signal.modelMaxAmplitude = value.model_max_amplitude();
        signal.humanMinAmplitude = value.human_min_amplitude();
        signal.humanMaxAmplitude = value.human_max_amplitude();

        switch (value.signal_category_id()) {
            case ::dfx::signals::v2::Category::MODEL:
                signal.category = SignalCategory::MODEL;
                break;
            case ::dfx::signals::v2::Category::ALGORITHM:
                signal.category = SignalCategory::ALGORITHM;
                break;
            case ::dfx::signals::v2::Category::CLASSIFIER:
                signal.category = SignalCategory::CLASSIFIER;
                break;
            case ::dfx::signals::v2::Category::SIGNAL:
                signal.category = SignalCategory::SIGNAL;
                break;
            case ::dfx::signals::v2::Category::SOURCE:
                signal.category = SignalCategory::SOURCE;
                break;
            case ::dfx::signals::v2::Category::UNKNOWN_SIGNAL_CATEGORY:
                signal.category = SignalCategory::UNKNOWN;
                break;
            default:
                signal.category = SignalCategory::UNKNOWN;
                break;
        }

        signalList.push_back(signal);
    }

    // Copy all the items we retrieved - this ensures signals state is consistent on failure
    // and allows client to pass existing items in list without us clearing.
    signals.insert(signals.end(), signalList.begin(), signalList.end());
    return CloudStatus(CLOUD_OK);
}

CloudStatus SignalGRPC::retrieveStudySignalIDs(const CloudConfig& config,
                                               const std::string& studyID,
                                               std::vector<string>& signalIDs)
{
    DFX_CLOUD_VALIDATOR_MACRO(SignalValidator, retrieveStudySignalIDs(config, studyID, signalIDs));

    dfx::studysignals::v2::RetrieveResponse response;
    dfx::studysignals::v2::RetrieveRequest request;
    request.set_id(studyID);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcStudySignalsStub->Retrieve(&context, request, &response));

    if (response.has_study_signal()) {
        const auto& studySignal = response.study_signal();
        // TODO: Need to finish the implementation
        return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
    } else {
        return CloudStatus(CLOUD_RECORD_NOT_FOUND);
    }

    return CloudStatus(CLOUD_OK);
}

CloudStatus SignalGRPC::retrieveSignalDetail(const CloudConfig& config,
                                             const std::string& signalID,
                                             Signal& signalDetail)
{
    DFX_CLOUD_VALIDATOR_MACRO(SignalValidator, retrieveSignalDetail(config, signalID, signalDetail));

    dfx::signals::v2::RetrieveResponse response;
    dfx::signals::v2::RetrieveRequest request;
    request.set_id(signalID);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcSignalsStub->Retrieve(&context, request, &response));

    if (response.has_signal()) {
        const auto& signalData = response.signal();

        signalDetail.name = signalData.name();
        signalDetail.version = signalData.version();
        signalDetail.description = signalData.description();
        signalDetail.unit = signalData.unit();
        signalDetail.modelMinAmplitude = signalData.model_min_amplitude();
        signalDetail.modelMaxAmplitude = signalData.model_max_amplitude();
        signalDetail.humanMinAmplitude = signalData.human_min_amplitude();
        signalDetail.humanMaxAmplitude = signalData.human_max_amplitude();
    } else {
        return CloudStatus(CLOUD_RECORD_NOT_FOUND);
    }

    return CloudStatus(CLOUD_OK);
}

CloudStatus SignalGRPC::retrieveSignalDetails(const CloudConfig& config,
                                              const std::list<std::string>& signalIDs,
                                              std::vector<Signal>& signalDetails)
{
    DFX_CLOUD_VALIDATOR_MACRO(SignalValidator, retrieveSignalDetails(config, signalIDs, signalDetails));

    dfx::signals::v2::RetrieveMultipleResponse response;
    dfx::signals::v2::RetrieveMultipleRequest request;
    for (const auto& signalID : signalIDs) {
        request.add_ids(signalID);
    }

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcSignalsStub->RetrieveMultiple(&context, request, &response));

    // TODO: Need to finish the implementation
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
    return CloudStatus(CLOUD_OK);
}
