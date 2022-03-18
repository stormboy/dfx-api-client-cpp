// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/StudyAPI.hpp"

#include "dfx/api/grpc/CloudGRPC.hpp"
#include "dfx/api/grpc/StudyGRPC.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

#include "CloudGRPCMacros.hpp"
#include <fmt/format.h>

using dfx::api::CloudAPI;
using dfx::api::CloudConfig;
using dfx::api::CloudStatus;
using dfx::api::Study;
using dfx::api::StudyAPI;
using dfx::api::StudyStatus;

using namespace dfx::api::grpc;
using namespace ::grpc;

StudyGRPC::StudyGRPC(const CloudConfig& config, const std::shared_ptr<CloudGRPC>& cloudGRPC)
{
    grpcStudiesStub = dfx::studies::v1::API::NewStub(cloudGRPC->getChannel(config));
}

// NOTE: gRPC does not support the studyConfig option
CloudStatus StudyGRPC::create(const CloudConfig& config,
                              const std::string& name,
                              const std::string& description,
                              const std::string& studyTemplateID,
                              const std::map<std::string, std::string>& studyConfig,
                              std::string& studyID)
{
    DFX_CLOUD_VALIDATOR_MACRO(StudyValidator, create(config, name, description, studyTemplateID, studyConfig, studyID));

    dfx::studies::v1::CreateResponse response;
    dfx::studies::v1::CreateRequest request;
    request.set_name(name);
    request.set_description(description);
    request.set_study_template_id(studyTemplateID);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcStudiesStub->Create(&context, request, &response));

    studyID = response.id();

    return CloudStatus(CLOUD_OK);
}

CloudStatus StudyGRPC::list(const CloudConfig& config,
                            const std::unordered_map<StudyFilter, std::string>& filters,
                            uint16_t offset,
                            std::vector<Study>& studies,
                            int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(StudyValidator, list(config, filters, offset, studies, totalCount));

    if (filters.size() > 0) {
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "Unexpected list filter key");
    }

    dfx::studies::v1::ListResponse response;
    dfx::studies::v1::ListRequest request;
    request.set_offset(offset);
    request.set_limit(config.listLimit);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcStudiesStub->List(&context, request, &response));
    totalCount = response.total();

    const auto numberStudies = response.studies_size();
    if (numberStudies == 0) {
        // It is possible with offset/limit to have no studies - but calling retrieveMultiple with an
        // empty set would return error so short-circuit here.
        return CloudStatus(CLOUD_OK);
    }

    std::vector<std::string> studyIDs;
    for (size_t index = 0; index < numberStudies; index++) {
        const auto& studyData = response.studies(static_cast<int>(index));
        studyIDs.push_back(studyData.id());
    }

    return retrieveMultiple(config, studyIDs, studies);
}

CloudStatus StudyGRPC::retrieve(const CloudConfig& config, const std::string& studyID, Study& study)
{
    DFX_CLOUD_VALIDATOR_MACRO(StudyValidator, retrieve(config, studyID, study));

    dfx::studies::v1::RetrieveResponse response;
    dfx::studies::v1::RetrieveRequest request;
    request.set_id(studyID);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcStudiesStub->Retrieve(&context, request, &response));

    if (response.has_study()) {
        const auto& studyDay = response.study();

        study.id = studyDay.id();
        study.name = studyDay.name();
        study.description = studyDay.description();
        study.templateID = studyDay.study_template_id();
        study.status = static_cast<StudyStatus>(studyDay.status());
        study.createdEpochSeconds = studyDay.created().seconds();
        study.updatedEpochSeconds = studyDay.updated().seconds();
    } else {
        return CloudStatus(CLOUD_RECORD_NOT_FOUND, fmt::format("Study id {} not found", studyID));
    }

    return CloudStatus(CLOUD_OK);
}

CloudStatus StudyGRPC::retrieveMultiple(const CloudConfig& config,
                                        const std::vector<std::string>& studyIDs,
                                        std::vector<Study>& studies)
{
    // Validate will occur by first retrieve call

    std::vector<Study> studyList;
    for (const auto& id : studyIDs) {
        Study study;
        auto status = retrieve(config, id, study);
        if (status.OK()) {
            studyList.push_back(study);
        } else {
            return status;
        }
    }

    // Copy all the items we retrieved - this ensures devices state consistent on failure
    // and allows client to pass existing items in list without us clearing.
    studies.insert(studies.end(), studyList.begin(), studyList.end());
    return CloudStatus(CLOUD_OK);
}

CloudStatus StudyGRPC::update(const CloudConfig& config,
                              const std::string& studyID,
                              const std::string& name,
                              const std::string& description,
                              StudyStatus status)
{
    DFX_CLOUD_VALIDATOR_MACRO(StudyValidator, update(config, studyID, name, description, status));

    ::google::protobuf::Empty response;
    dfx::studies::v1::UpdateRequest request;
    request.set_id(studyID);
    request.set_name(name);
    request.set_description(description);
    request.set_status(static_cast<::dfx::studies::v1::Status>(status));

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcStudiesStub->Update(&context, request, &response));

    return CloudStatus(CLOUD_OK);
}

CloudStatus StudyGRPC::remove(const CloudConfig& config, const std::string& studyID)
{
    DFX_CLOUD_VALIDATOR_MACRO(StudyValidator, remove(config, studyID));

    ::google::protobuf::Empty response;
    dfx::studies::v1::RemoveRequest request;
    request.set_id(studyID);

    ClientContext context;
    CloudGRPC::initializeClientContext(config, context, config.authToken);

    MACRO_RETURN_ERROR_IF_GRPC_STATUS_NOT_OK(grpcStudiesStub->Remove(&context, request, &response));

    return CloudStatus(CLOUD_OK);
}

CloudStatus StudyGRPC::retrieveStudyConfig(const CloudConfig& config,
                                           const std::string& studyID,
                                           const std::string& sdkID,
                                           const std::string& currentHashID,
                                           std::vector<uint8_t>& studyData,
                                           std::string& hashID)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "gRPC", "study config retrieve"));
}

CloudStatus StudyGRPC::retrieveStudyTypes(const CloudConfig& config,
                                          const StudyStatus status,
                                          std::list<StudyType>& studyTypes)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "gRPC", "study retrieve types"));
};

CloudStatus StudyGRPC::listStudyTemplates(const CloudConfig& config,
                                          const StudyStatus status,
                                          const std::string& type,
                                          std::list<StudyTemplate>& studyTemplates)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "gRPC", "study list templates"));
};