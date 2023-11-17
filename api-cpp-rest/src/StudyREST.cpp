// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/rest/StudyREST.hpp"
#include "dfx/api/rest/CloudREST.hpp"
#include "dfx/api/validator/CloudValidator.hpp"
#include "dfx/api/web/WebServiceDetail.hpp"

#include "nlohmann/json.hpp"
#include <sstream>
#include <string>

using namespace dfx::api;
using namespace dfx::api::rest;

CloudStatus StudyREST::create(const CloudConfig& config,
                              const std::string& name,
                              const std::string& description,
                              const std::string& templateID,
                              const std::map<std::string, std::string>& studyConfig,
                              std::string& studyID)
{
    DFX_CLOUD_VALIDATOR_MACRO(StudyValidator, create(config, name, description, templateID, studyConfig, studyID));

    nlohmann::json request;
    nlohmann::json response;

    request["Name"] = name;
    request["Description"] = description;
    request["StudyTemplateID"] = templateID;
    auto it = studyConfig.begin();
    while (it != studyConfig.end()) {
        request["Config"][it->first] = it->second;
        it++;
    };

    // https://dfxapiversion10.docs.apiary.io/#reference/0/studies/create
    auto result = CloudREST::performRESTCall(config, web::Studies::Create, config.authToken, {}, request, response);
    if (result.OK()) {
        studyID = response["ID"].get<std::string>();
    }
    return result;
}

CloudStatus StudyREST::list(const CloudConfig& config,
                            const std::unordered_map<StudyFilter, std::string>& filters,
                            uint16_t offset,
                            std::vector<Study>& studies,
                            int16_t& totalCount)
{
    DFX_CLOUD_VALIDATOR_MACRO(StudyValidator, list(config, filters, offset, studies, totalCount));

    totalCount = -1; // Return unknown -1, zero would be a literal zero

    if (!filters.empty()) {
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, "Unsupported filter provided");
    }

    nlohmann::json request;
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/studies/list
    auto result = CloudREST::performRESTCall(config, web::Studies::List, config.authToken, {}, request, response);

    if (result.OK()) {
        const auto numberStudies = response.size();
        if (numberStudies == 0) {
            // It is possible with offset/limit to have no studies - but calling retrieveMultiple with an
            // empty set would return error so short-circuit here.
            return CloudStatus(CLOUD_OK);
        }

        if (numberStudies > 0) {
            // First element assuming there is one will have a TotalCount field which makes for
            // a non-uniform JSON schema so custom decode here
            if (response[0].contains("TotalCount")) {
                totalCount = response[0]["TotalCount"];
            }
        }

        std::vector<std::string> studyIDs;
        for (auto& item : response) {
            studyIDs.push_back(item["ID"].get<std::string>());
        }

        return retrieveMultiple(config, studyIDs, studies);
    }

    return result;
}

CloudStatus StudyREST::retrieve(const CloudConfig& config, const std::string& studyID, Study& study)
{
    DFX_CLOUD_VALIDATOR_MACRO(StudyValidator, retrieve(config, studyID, study));

    nlohmann::json request;
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/studies/retrieve
    auto result =
        CloudREST::performRESTCall(config, web::Studies::Retrieve, config.authToken, {studyID}, request, response);

    if (result.OK()) {
        study = response;
    }

    return CloudStatus(CLOUD_OK);
}

CloudStatus StudyREST::retrieveMultiple(const CloudConfig& config,
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

CloudStatus StudyREST::update(const CloudConfig& config,
                              const std::string& studyID,
                              const std::string& name,
                              const std::string& description,
                              StudyStatus status)
{
    DFX_CLOUD_VALIDATOR_MACRO(StudyValidator, update(config, studyID, name, description, status));

    nlohmann::json request = {{"Name", name},
                              {"StatusID", StudyStatusMapper::toString.at(status)},
                              {"Description", description},
                              {"Config", "{}"}};
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/studies/update
    return CloudREST::performRESTCall(config, web::Studies::Update, config.authToken, {studyID}, request, response);
}

CloudStatus StudyREST::remove(const CloudConfig& config, const std::string& studyID)
{
    DFX_CLOUD_VALIDATOR_MACRO(StudyValidator, remove(config, studyID));
    nlohmann::json request;
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/studies/delete
    return CloudREST::performRESTCall(config, web::Studies::Remove, config.authToken, {studyID}, request, response);
}

CloudStatus StudyREST::retrieveStudyConfig(const CloudConfig& config,
                                           const std::string& studyID,
                                           const std::string& sdkID,
                                           const std::string& currentHashID,
                                           std::vector<uint8_t>& studyData,
                                           std::string& hashID)
{
    DFX_CLOUD_VALIDATOR_MACRO(StudyValidator,
                              retrieveStudyConfig(config, studyID, sdkID, currentHashID, studyData, hashID));

    nlohmann::json request = {{"StudyID", studyID}, {"SDKID", sdkID}, {"MD5Hash", currentHashID}};
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/studies/retrieve-sdk-study-config-data
    auto result = CloudREST::performRESTCall(config, web::Studies::GetConfig, config.authToken, {}, request, response);
    const int HTTP_304_Not_Modified = 304;
    if (result.OK()) {
        studyData = response["ConfigFile"]["data"].get<std::vector<uint8_t>>();
        hashID = response["MD5Hash"].get<std::string>();
    } else if (result.getProtocolCode() == HTTP_304_Not_Modified) {
        // This is expected if the currentHashID matched what we were provided.
        hashID = currentHashID;
        return CloudStatus(CLOUD_OK); // HTTP 304 really means everything is OK for this call
    }
    return result;
}

CloudStatus StudyREST::retrieveStudyTypes(const CloudConfig& config,
                                          const StudyStatus status,
                                          std::list<StudyType>& studyTypes)
{
    DFX_CLOUD_VALIDATOR_MACRO(StudyValidator, retrieveStudyTypes(config, status, studyTypes));

    nlohmann::json request;
    nlohmann::json response;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/studies/types
    auto result = CloudREST::performRESTCall(
        config, web::Studies::Types, config.authToken, {}, StudyStatusMapper::getString(status), request, response);

    if (result.OK()) {
        for (auto& item : response) {
            StudyType studyType;
            studyType.id = item["ID"].get<std::string>();
            studyType.name = item["Name"].get<std::string>();
            studyType.description = item["Description"].get<std::string>();
            studyType.status = StudyStatusMapper::toEnum.at(item["StatusID"].get<std::string>());
            studyTypes.push_back(studyType);
        }
    }

    return CloudStatus(CLOUD_OK);
}

CloudStatus StudyREST::listStudyTemplates(const CloudConfig& config,
                                          const StudyStatus status,
                                          const std::string& type,
                                          std::list<StudyTemplate>& studyTemplates)
{
    DFX_CLOUD_VALIDATOR_MACRO(StudyValidator, listStudyTemplates(config, status, type, studyTemplates));

    nlohmann::json request;
    nlohmann::json response;

    std::stringstream urlQuery;
    urlQuery << "Status=" << StudyStatusMapper::toString.at(status) << "&Type" << type;

    // https://dfxapiversion10.docs.apiary.io/#reference/0/studies/list-templates
    auto result = CloudREST::performRESTCall(
        config, web::Studies::RetrieveTemplates, config.authToken, {}, urlQuery.str(), request, response);

    if (result.OK()) {
        for (auto& item : response) {
            StudyTemplate studyTemplate;
            studyTemplate.id = item["ID"].get<std::string>();
            studyTemplate.name = item["Name"].get<std::string>();
            studyTemplate.description = item["Description"].get<std::string>();
            studyTemplate.status = StudyStatusMapper::toEnum.at(item["StatusID"].get<std::string>());
            if (item.contains("BundleID")) {
                studyTemplate.bundleID = item["BundleID"].get<std::string>();
            }

            studyTemplate.config = item["Config"].dump();
            studyTemplate.studyTypeID = item["StudyTypeID"].get<std::string>();
            for (auto& signal : item["Signals"]) {
                studyTemplate.signalIDS.push_back(signal);
            }
            studyTemplate.createdEpochSeconds = item["Created"];
            studyTemplates.push_back(studyTemplate);
        }
    }

    return CloudStatus(CLOUD_OK);
}
