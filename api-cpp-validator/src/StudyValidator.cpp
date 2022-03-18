// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/validator/StudyValidator.hpp"

#include "CloudValidatorMacros.hpp"

using namespace dfx::api;
using namespace dfx::api::validator;

const StudyValidator& StudyValidator::instance()
{
    static const StudyValidator instance;
    return instance;
}

CloudStatus StudyValidator::create(const CloudConfig& config,
                                   const std::string& name,
                                   const std::string& description,
                                   const std::string& studyTemplateID,
                                   const std::map<std::string, std::string>& studyConfig,
                                   std::string& studyID)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(name);
    MACRO_RETURN_ERROR_IF_EMPTY(description);
    MACRO_RETURN_ERROR_IF_EMPTY(studyTemplateID);
    return CloudStatus(CLOUD_OK);
}

CloudStatus StudyValidator::list(const CloudConfig& config,
                                 const std::unordered_map<StudyFilter, std::string>& filters,
                                 uint16_t offset,
                                 std::vector<Study>& studies,
                                 int16_t& totalCount)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    return CloudStatus(CLOUD_OK);
}

CloudStatus StudyValidator::retrieve(const CloudConfig& config, const std::string& studyID, Study& study)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(studyID);
    return CloudStatus(CLOUD_OK);
}

CloudStatus StudyValidator::retrieveMultiple(const CloudConfig& config,
                                             const std::vector<std::string>& studyIDs,
                                             std::vector<Study>& studies)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(studyIDs);
    return CloudStatus(CLOUD_OK);
}

CloudStatus StudyValidator::update(const CloudConfig& config,
                                   const std::string& studyID,
                                   const std::string& name,
                                   const std::string& description,
                                   StudyStatus status)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(studyID);
    MACRO_RETURN_ERROR_IF_EMPTY(name);
    MACRO_RETURN_ERROR_IF_EMPTY(description);
    return CloudStatus(CLOUD_OK);
}

CloudStatus StudyValidator::remove(const CloudConfig& config, const std::string& studyID)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(studyID);
    return CloudStatus(CLOUD_OK);
}

CloudStatus StudyValidator::retrieveStudyConfig(const CloudConfig& config,
                                                const std::string& studyID,
                                                const std::string& sdkID,
                                                const std::string& currentHashID,
                                                std::vector<uint8_t>& studyData,
                                                std::string& hashID)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(studyID);
    MACRO_RETURN_ERROR_IF_EMPTY(sdkID);
    MACRO_RETURN_ERROR_IF_EMPTY(currentHashID);
    return CloudStatus(CLOUD_OK);
}

CloudStatus StudyValidator::retrieveStudyTypes(const CloudConfig& config,
                                               const StudyStatus status,
                                               std::list<StudyType>& studyTypes)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    return CloudStatus(CLOUD_OK);
}

CloudStatus StudyValidator::listStudyTemplates(const CloudConfig& config,
                                               const StudyStatus status,
                                               const std::string& type,
                                               std::list<StudyTemplate>& studyTemplates)
{
    MACRO_RETURN_ERROR_IF_NO_USER_TOKEN(config);
    MACRO_RETURN_ERROR_IF_EMPTY(type);
    return CloudStatus(CLOUD_OK);
}
