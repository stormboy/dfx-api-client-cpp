// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/StudyAPI.hpp"

using namespace dfx::api;

CloudStatus StudyAPI::create(const CloudConfig& config,
                             const std::string& name,
                             const std::string& description,
                             const std::string& studyTemplateID,
                             const std::map<std::string, std::string>& studyConfig,
                             std::string& studyID)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus StudyAPI::list(const CloudConfig& config,
                           const std::unordered_map<StudyFilter, std::string>& filters,
                           uint16_t offset,
                           std::vector<Study>& studies,
                           int16_t& totalCount)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus StudyAPI::retrieve(const CloudConfig& config, const std::string& studyID, Study& study)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus StudyAPI::retrieveMultiple(const CloudConfig& config,
                                       const std::vector<std::string>& studyIDs,
                                       std::vector<Study>& studies)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus StudyAPI::update(const CloudConfig& config,
                             const std::string& studyID,
                             const std::string& name,
                             const std::string& description,
                             StudyStatus status)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus StudyAPI::remove(const CloudConfig& config, const std::string& studyID)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus StudyAPI::retrieveStudyConfig(const CloudConfig& config,
                                          const std::string& studyID,
                                          const std::string& sdkID,
                                          const std::string& currentHashID,
                                          std::vector<uint8_t>& studyData,
                                          std::string& hashID)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus StudyAPI::retrieveStudyTypes(const CloudConfig& config,
                                         const StudyStatus status,
                                         std::list<StudyType>& studyTypes)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus StudyAPI::listStudyTemplates(const CloudConfig& config,
                                         const StudyStatus status,
                                         const std::string& type,
                                         std::list<StudyTemplate>& studyTemplates)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}
