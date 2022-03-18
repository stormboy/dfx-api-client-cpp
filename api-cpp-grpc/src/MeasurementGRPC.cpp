// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/SignalAPI.hpp"

#include "dfx/api/grpc/CloudGRPC.hpp"
#include "dfx/api/grpc/MeasurementGRPC.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

#include "dfx/api/types/FilterHelpers.hpp"

#include "dfx/measurements/v2/measurements.grpc.pb.h"

#include "CloudGRPCMacros.hpp"

using dfx::api::CloudAPI;
using dfx::api::CloudConfig;
using dfx::api::CloudStatus;
using dfx::api::Measurement;
using dfx::api::MeasurementAPI;

using namespace dfx::api::grpc;
using namespace ::grpc;

MeasurementGRPC::MeasurementGRPC(const CloudConfig& config, const std::shared_ptr<CloudGRPC>& cloudGRPC)
{
    grpcMeasurementsStub = dfx::measurements::v2::API::NewStub(cloudGRPC->getChannel(config));
}

CloudStatus MeasurementGRPC::list(const CloudConfig& config,
                                  const std::unordered_map<MeasurementFilter, std::string>& filters,
                                  uint16_t offset,
                                  std::vector<Measurement>& measurements,
                                  int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(MeasurementValidator, list(config, filters, offset, measurements, totalCount));

    dfx::measurements::v2::ListResponse response;
    dfx::measurements::v2::ListRequest request;

    auto profileID = dfx::api::getFilterString(filters, MeasurementFilter::UserProfileId, "");
    if (!profileID.empty()) {
        request.set_profile_id(profileID);
    }

    auto startDate = dfx::api::getFilterInt64(filters, MeasurementFilter::StartDate, 0);
    if (startDate != 0) {
        request.mutable_start_date()->set_seconds(startDate);
    }

    auto endDate = dfx::api::getFilterInt64(filters, MeasurementFilter::EndDate, 0);
    if (endDate != 0) {
        request.mutable_end_date()->set_seconds(endDate);
    }

    request.set_limit(config.listLimit);
    request.set_offset(offset);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcMeasurementsStub->List(&context, request, &response));
    totalCount = response.total();

    const auto numberMeasurements = response.measurements_size();
    if (numberMeasurements == 0) {
        // It is possible with offset/limit to have no measurements - but calling retrieveMultiple with an
        // empty set would return error so short-circuit here.
        return CloudStatus(CLOUD_OK);
    }

    bool fullObject = getFilterBool(filters, MeasurementFilter::FullObject, false);
    if (fullObject) {
        std::vector<std::string> measurementIDs;
        for (auto index = 0; index < numberMeasurements; index++) {
            const auto& measurementData = response.measurements(static_cast<int>(index));
            measurementIDs.push_back(measurementData.id());
        }
        return retrieveMultiple(config, measurementIDs, measurements);
    } else {
        for (auto index = 0; index < numberMeasurements; index++) {
            const auto& measurementData = response.measurements(static_cast<int>(index));
            Measurement measurement{};
            measurement.id = measurementData.id();
            measurement.status = static_cast<MeasurementStatus>(measurementData.status());
            measurements.push_back(measurement);
        }
        return CloudStatus(CLOUD_OK);
    }
}

CloudStatus MeasurementGRPC::retrieve(const CloudConfig& config,
                                      const std::string& measurementID,
                                      Measurement& measurementData)
{
    DFX_CLOUD_VALIDATOR_MACRO(MeasurementValidator, retrieve(config, measurementID, measurementData));

    dfx::measurements::v2::RetrieveResponse response;
    dfx::measurements::v2::RetrieveRequest request;
    request.set_id(measurementID);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcMeasurementsStub->Retrieve(&context, request, &response));

    if (response.has_measurement()) {
        const auto& measurement = response.measurement();

        measurementData.id = measurement.id();
        measurementData.studyID = measurement.study_id();
        measurementData.deviceID = measurement.device_id();
        measurementData.userProfileID = measurement.profile_id();
        measurementData.status = static_cast<MeasurementStatus>(measurement.status());
    }

    return CloudStatus(CLOUD_OK);
}

CloudStatus MeasurementGRPC::retrieveMultiple(const CloudConfig& config,
                                              const std::vector<std::string>& measurementIDs,
                                              std::vector<Measurement>& measurements)
{
    DFX_CLOUD_VALIDATOR_MACRO(MeasurementValidator, retrieveMultiple(config, measurementIDs, measurements));

    dfx::measurements::v2::RetrieveMultipleResponse response;
    dfx::measurements::v2::RetrieveMultipleRequest request;
    for (const auto& measurementID : measurementIDs) {
        request.add_ids(measurementID);
    }

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcMeasurementsStub->RetrieveMultiple(&context, request, &response));

    const auto numberMeasurements = response.measurements_size();
    for (size_t index = 0; index < numberMeasurements; index++) {
        const auto& measurementData = response.measurements(static_cast<int>(index));

        Measurement measurement;
        measurement.id = measurementData.id();
        measurement.studyID = measurementData.study_id();
        measurement.userProfileID = measurementData.profile_id();
        measurement.status = static_cast<MeasurementStatus>(measurementData.status());
        measurement.createdEpochSeconds = measurementData.created().seconds();
        measurement.updatedEpochSeconds = measurementData.updated().seconds();
        measurements.push_back(measurement);
    }

    return CloudStatus(CLOUD_OK);
}
