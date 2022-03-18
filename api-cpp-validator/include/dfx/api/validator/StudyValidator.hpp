// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_STUDY_VALIDATOR_H
#define DFX_API_CLOUD_STUDY_VALIDATOR_H

#include "dfx/api/StudyAPI.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

namespace dfx::api::validator
{

class StudyValidator : public StudyAPI
{
public:
    StudyValidator() = default;

    ~StudyValidator() override = default;

    CloudStatus create(const CloudConfig& config,
                       const std::string& name,
                       const std::string& description,
                       const std::string& studyTemplateID,
                       const std::map<std::string, std::string>& studyConfig,
                       std::string& studyID) override;

    CloudStatus list(const CloudConfig& config,
                     const std::unordered_map<StudyFilter, std::string>& filters,
                     uint16_t offset,
                     std::vector<Study>& studies,
                     int16_t& totalCount) override;

    CloudStatus retrieve(const CloudConfig& config, const std::string& studyID, Study& study) override;

    CloudStatus retrieveMultiple(const CloudConfig& config,
                                 const std::vector<std::string>& studyIDs,
                                 std::vector<Study>& studies) override;

    CloudStatus update(const CloudConfig& config,
                       const std::string& studyID,
                       const std::string& name,
                       const std::string& description,
                       StudyStatus status) override;

    CloudStatus remove(const CloudConfig& config, const std::string& studyID) override;

    CloudStatus retrieveStudyConfig(const CloudConfig& config,
                                    const std::string& studyID,
                                    const std::string& sdkID,
                                    const std::string& currentHashID,
                                    std::vector<uint8_t>& studyData,
                                    std::string& hashID) override;

    CloudStatus retrieveStudyTypes(const CloudConfig& config,
                                   const StudyStatus status,
                                   std::list<StudyType>& studyTypes) override;

    CloudStatus listStudyTemplates(const CloudConfig& config,
                                   const StudyStatus status,
                                   const std::string& type,
                                   std::list<StudyTemplate>& studyTemplates) override;

    static const StudyValidator& instance();
};

} // namespace dfx::api::validator

#endif // DFX_API_CLOUD_STUDY_VALIDATOR_H
