// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_REST_API_H
#define DFX_API_CLOUD_REST_API_H

#include "dfx/api/CloudAPI.hpp"
#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/web/WebServiceDetail.hpp"

#include <list>
#include <memory>
#include <string>

namespace dfx::api::rest
{

/**
 * @class CloudREST CloudREST.h "dfx/api/rest/CloudREST.h"
 *
 * @brief CloudRestAPI provides a C++ class which communicates with the Nuralogix
 *        DFX API version 2 servers using REST communications.
 *
 * The internal implementation follows the V2 API documentation.
 *
 * @see https://dfxapiversion10.docs.apiary.io.
 */
class DFXCLOUD_EXPORT CloudREST : public CloudAPI
{
public:
    explicit CloudREST(const CloudConfig& config);

    ~CloudREST() override = default;

    CloudStatus connect(const CloudConfig& config) override;

    const std::string& getTransportType() override;

    CloudStatus getServerStatus(CloudConfig& config, std::string& response) override;

    // *********************************************************************************
    // AUTHENTICATION SECTION
    // *********************************************************************************
    CloudStatus login(CloudConfig& config) override;

    CloudStatus loginWithToken(CloudConfig& config, std::string& token) override;

    CloudStatus logout(CloudConfig& config) override;

    CloudStatus registerDevice(CloudConfig& config,
                               const std::string& appName,
                               const std::string& appVersion,
                               const uint16_t tokenExpiresInSeconds,
                               const std::string& tokenSubject) override;

    CloudStatus unregisterDevice(CloudConfig& config) override;

    CloudStatus verifyToken(const CloudConfig& config, std::string& response) override;

    CloudStatus renewToken(const CloudConfig& config, std::string& token, std::string& refreshToken) override;

    CloudStatus switchEffectiveOrganization(CloudConfig& config, const std::string& organizationID) override;

    // *********************************************************************************
    // SERVICES SECTION
    // *********************************************************************************

    std::shared_ptr<DeviceAPI> device(const CloudConfig& config) override;

    std::shared_ptr<LicenseAPI> license(const CloudConfig& config) override;

    std::shared_ptr<MeasurementAPI> measurement(const CloudConfig& config) override;

    std::shared_ptr<MeasurementStreamAPI> measurementStream(const CloudConfig& config) override;

    std::shared_ptr<OrganizationAPI> organization(const CloudConfig& config) override;

    std::shared_ptr<ProfileAPI> profile(const CloudConfig& config) override;

    std::shared_ptr<SignalAPI> signal(const CloudConfig& config) override;

    std::shared_ptr<StudyAPI> study(const CloudConfig& config) override;

    std::shared_ptr<UserAPI> user(const CloudConfig& config) override;

private:
    friend class DeviceREST;

    friend class LicenseREST;

    friend class MeasurementREST;

    friend class OrganizationREST;

    friend class ProfileREST;

    friend class SignalREST;

    friend class StudyREST;

    friend class UserREST;

    std::string getAuthToken(const CloudConfig& config);

protected:
    static CloudStatus performRESTCall(const CloudConfig& config,
                                       const dfx::api::web::WebServiceDetail& details,
                                       const std::string& authToken,
                                       const std::vector<std::string>& urlArgs,
                                       const nlohmann::json& payload,
                                       nlohmann::json& response);

    static CloudStatus performRESTCall(const CloudConfig& config,
                                       const dfx::api::web::WebServiceDetail& details,
                                       const std::string& authToken,
                                       const std::vector<std::string>& urlArgs,
                                       const std::string& query,
                                       const nlohmann::json& payload,
                                       nlohmann::json& response);

    static std::string buildListFilterQuery(const std::map<std::string, std::string>* filterCriteria,
                                            uint16_t offset,
                                            uint16_t limit);
};

} // namespace dfx::api::rest

#endif // DFX_CLOUD_REST_API_H
