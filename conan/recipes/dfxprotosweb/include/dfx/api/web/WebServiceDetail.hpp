
// ***** This file was auto-generated "2023-07-12 19:10" from "routes.json" *****

// Generated by conanfile package dfxprotos script build_service_detail.py

/*
 * Copyright (c) 2016-2021 NuraLogix.
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */

#ifndef DFX_API_WEB_WEBSERVICEDETAIL_H
#define DFX_API_WEB_WEBSERVICEDETAIL_H

#include <string>

// The WebServiceDetail variables are namespaced to read convenient when used
// and ensure they don't collide. For example, the definition to perform a
// user login will map to the variable:
// 
// dfx::api::web::Users::Login 
// 
namespace dfx {
  namespace api {
    namespace web {

      /**
       * @brief WebServiceDetail contains the necessary information to communicate
       * with either the REST or WebSocket DFX API backends.
       */
      typedef struct WebServiceDetail {
        int wsCode;              // websocket code, use fmt::format("{:<4}",s.wsCode)
        std::string httpOption;  // GET, POST, DELETE, PATCH, CONNECT
        std::string urlPath;     // URL path with fmt {} holders for arguments 
        int urlArgCount;         // Number of expected arguments, in order of the
      } WebServiceDetail;        // comment that trails the urlPath

      namespace General {
        static const WebServiceDetail Status = { 100, "GET", "status", 0 };  
        static const WebServiceDetail MimeTypes = { 101, "GET", "mimes", 0 };  
        static const WebServiceDetail Roles = { 102, "GET", "roles", 0 };  
        static const WebServiceDetail Statuses = { 104, "GET", "statuses", 0 };  
        static const WebServiceDetail VerifyToken = { 107, "GET", "auth", 0 };  
        static const WebServiceDetail Regions = { 108, "GET", "regions", 0 };  
        static const WebServiceDetail RetrieveConsent = { 109, "GET", "consents", 0 };  
        static const WebServiceDetail CreateConsent = { 110, "POST", "consents", 0 };  
        static const WebServiceDetail UpdateConsent = { 111, "PATCH", "consents", 0 };  
        static const WebServiceDetail DeleteConsent = { 112, "DELETE", "consents", 0 };  
      }

      namespace Users {
        static const WebServiceDetail Create = { 200, "POST", "users", 0 };  
        static const WebServiceDetail Login = { 201, "POST", "users/auth", 0 };  
        static const WebServiceDetail MfaSecret = { 221, "POST", "users/mfa/secret", 0 };  
        static const WebServiceDetail EnableMfa = { 222, "POST", "users/mfa", 0 };  
        static const WebServiceDetail DisableMfa = { 223, "DELETE", "users/mfa", 0 };  
        static const WebServiceDetail Retrieve = { 202, "GET", "users", 0 };  
        static const WebServiceDetail Remove = { 206, "DELETE", "users", 0 };  
        static const WebServiceDetail Update = { 208, "PATCH", "users", 0 };  
        static const WebServiceDetail SendReset = { 209, "PATCH", "users/sendreset", 0 };  
        static const WebServiceDetail ResetPassword = { 210, "PATCH", "users/reset", 0 };  
        static const WebServiceDetail GetRole = { 211, "GET", "users/role", 0 };  
        static const WebServiceDetail Verify = { 212, "POST", "users/verify", 0 };  
        static const WebServiceDetail VerificationCode = { 213, "GET", "users/verificationCode/{}/{}", 2 };  // Args: ID, OrgKey
        static const WebServiceDetail SendResetMobile = { 214, "PATCH", "users/sendreset/mobile", 0 };  
        static const WebServiceDetail Signup = { 215, "POST", "users/signup", 0 };  
        static const WebServiceDetail RequestLoginCode = { 216, "GET", "users/auth/code/{}/{}", 2 };  // Args: OrgKey, PhoneNumber
        static const WebServiceDetail LoginWithCode = { 217, "POST", "users/auth/code", 0 };  
        static const WebServiceDetail Sso = { 218, "POST", "users/auth/sso", 0 };  
        static const WebServiceDetail RemoveUserMeasurements = { 219, "DELETE", "users/{}/measurements", 1 };  // Args: ID
        static const WebServiceDetail ChangePassword = { 220, "POST", "users/changepassword", 0 };  
        static const WebServiceDetail DisableMfaForUser = { 224, "DELETE", "users/{}/mfa", 1 };  // Args: ID
        static const WebServiceDetail RenewToken = { 225, "POST", "users/auth/renew", 0 };  
        static const WebServiceDetail Logout = { 226, "DELETE", "users/auth", 0 };  
        static const WebServiceDetail RecoveryVerificationCode = { 228, "GET", "users/recoveryVerificationCode/{}", 1 };  // Args: ID
      }

      namespace Meta {
        static const WebServiceDetail Retrieve = { 300, "GET", "meta/{}", 1 };  // Args: Namespace
        static const WebServiceDetail RetrieveByType = { 301, "GET", "meta/{}/{}/{}", 3 };  // Args: Type, ID, Namespace
        static const WebServiceDetail Update = { 302, "PATCH", "meta/{}", 1 };  // Args: Namespace
        static const WebServiceDetail UpdateByType = { 303, "PATCH", "meta/{}/{}/{}", 3 };  // Args: Type, ID, Namespace
      }

      namespace Profiles {
        static const WebServiceDetail Create = { 400, "POST", "users/profiles", 0 };  
        static const WebServiceDetail Retrieve = { 401, "GET", "users/profiles/{}", 1 };  // Args: ID
        static const WebServiceDetail List = { 402, "GET", "users/profiles", 0 };  
        static const WebServiceDetail ListByUser = { 403, "GET", "users/{}/profiles", 1 };  // Args: ID
        static const WebServiceDetail Remove = { 404, "DELETE", "users/profiles/{}", 1 };  // Args: ID
        static const WebServiceDetail Update = { 405, "PATCH", "users/profiles/{}", 1 };  // Args: ID
      }

      namespace Measurements {
        static const WebServiceDetail Retrieve = { 500, "GET", "measurements/{}", 1 };  // Args: ID
        static const WebServiceDetail List = { 501, "GET", "measurements", 0 };  
        static const WebServiceDetail Create = { 504, "POST", "measurements", 0 };  
        static const WebServiceDetail Data = { 506, "POST", "measurements/{}/data", 1 };  // Args: ID
        static const WebServiceDetail Remove = { 507, "DELETE", "measurements/{}", 1 };  // Args: ID
        static const WebServiceDetail SubscribeResults = { 510, "CONNECT", "measurements/{}/results/", 1 };  // Args: ID
        static const WebServiceDetail Reprocess = { 512, "PATCH", "measurements/reprocess", 0 };  
        static const WebServiceDetail AddTags = { 515, "POST", "measurements/{}/tags", 1 };  // Args: ID
        static const WebServiceDetail SaveResults = { 517, "POST", "measurements/{}/results", 1 };  // Args: ID
        static const WebServiceDetail AddSurveyData = { 518, "POST", "survey/{}", 1 };  // Args: StudyID
        static const WebServiceDetail BugReport = { 520, "POST", "measurements/{}/bug-report", 1 };  // Args: ID
        static const WebServiceDetail RetrieveBugReport = { 521, "GET", "measurements/{}/bug-report", 1 };  // Args: ID
        static const WebServiceDetail MeasurementEnd = { 522, "POST", "measurements/{}/end", 1 };  // Args: ID
      }

      namespace Groups {
        static const WebServiceDetail List = { 600, "GET", "groups", 0 };  
        static const WebServiceDetail Types = { 601, "GET", "groups/types", 0 };  
        static const WebServiceDetail Retrieve = { 602, "GET", "groups/{}", 1 };  // Args: ID
        static const WebServiceDetail GetUsers = { 603, "GET", "groups/{}/users", 1 };  // Args: ID
        static const WebServiceDetail Create = { 604, "POST", "groups", 0 };  
        static const WebServiceDetail AddUsers = { 605, "POST", "groups/{}/users", 1 };  // Args: ID
        static const WebServiceDetail Update = { 606, "PATCH", "groups/{}", 1 };  // Args: ID
        static const WebServiceDetail Remove = { 607, "DELETE", "groups/{}", 1 };  // Args: ID
        static const WebServiceDetail RemoveUsers = { 608, "DELETE", "groups/{}/users", 1 };  // Args: ID
      }

      namespace Organizations {
        static const WebServiceDetail Retrieve = { 700, "GET", "organizations", 0 };  
        static const WebServiceDetail Users = { 702, "GET", "organizations/users", 0 };  
        static const WebServiceDetail ListMeasurements = { 703, "GET", "organizations/measurements", 0 };  
        static const WebServiceDetail RetrieveMeasurement = { 704, "GET", "organizations/measurements/{}", 1 };  // Args: ID
        static const WebServiceDetail RegisterLicense = { 705, "POST", "organizations/licenses", 0 };  
        static const WebServiceDetail UnregisterLicense = { 706, "DELETE", "organizations/licenses", 0 };  
        static const WebServiceDetail RetrieveLogo = { 708, "GET", "organizations/{}/logo", 1 };  // Args: ID
        static const WebServiceDetail ListProfiles = { 710, "GET", "organizations/profiles", 0 };  
        static const WebServiceDetail RetrieveProfile = { 711, "GET", "organizations/profiles/{}", 1 };  // Args: ID
        static const WebServiceDetail RetrieveUser = { 712, "GET", "organizations/users/{}", 1 };  // Args: ID
        static const WebServiceDetail CreateUser = { 713, "POST", "organizations/users", 0 };  
        static const WebServiceDetail UpdateUser = { 714, "PATCH", "organizations/users/{}", 1 };  // Args: ID
        static const WebServiceDetail RemoveUser = { 715, "DELETE", "organizations/users/{}", 1 };  // Args: ID
        static const WebServiceDetail UpdateProfile = { 716, "PATCH", "organizations/profiles/{}", 1 };  // Args: ID
        static const WebServiceDetail Login = { 717, "POST", "organizations/auth", 0 };  
        static const WebServiceDetail LoginWithToken = { 718, "POST", "organizations/auth/token", 0 };  
        static const WebServiceDetail ContactUser = { 719, "POST", "organizations/users/contact", 0 };  
        static const WebServiceDetail Update = { 720, "PATCH", "organizations", 0 };  
        static const WebServiceDetail RemoveOrgMeasurements = { 721, "DELETE", "organizations/{}/measurements", 1 };  // Args: ID
        static const WebServiceDetail RemovePartnerMeasurements = { 722, "DELETE", "organizations/{}/partners/{}/measurements", 2 };  // Args: ID, PartnerID
        static const WebServiceDetail GetDefaultPasswordPolicy = { 723, "GET", "organizations/policy/password", 0 };  
      }

      namespace Studies {
        static const WebServiceDetail Types = { 800, "GET", "studies/types", 0 };  
        static const WebServiceDetail RetrieveTemplates = { 801, "GET", "studies/templates", 0 };  
        static const WebServiceDetail Assets = { 803, "GET", "studies/templates/{}/assets", 1 };  // Args: ID
        static const WebServiceDetail Retrieve = { 804, "GET", "studies/{}", 1 };  // Args: ID
        static const WebServiceDetail List = { 805, "GET", "studies", 0 };  
        static const WebServiceDetail Create = { 806, "POST", "studies", 0 };  
        static const WebServiceDetail Update = { 807, "PATCH", "studies/{}", 1 };  // Args: ID
        static const WebServiceDetail Remove = { 808, "DELETE", "studies/{}", 1 };  // Args: ID
        static const WebServiceDetail GetConfig = { 809, "POST", "studies/sdkconfig", 0 };  
        static const WebServiceDetail GetConfigHash = { 810, "POST", "studies/sdkconfig/hash", 0 };  
        static const WebServiceDetail RemoveStudyMeasurements = { 811, "DELETE", "studies/{}/measurements", 1 };  // Args: ID
        static const WebServiceDetail GetStudySurveys = { 812, "GET", "studies/{}/surveys", 1 };  // Args: ID
        static const WebServiceDetail GetSurveySchema = { 813, "GET", "studies/{}/surveys/{}/language/{}", 3 };  // Args: ID, SignalID, Language
        static const WebServiceDetail SubmitSurvey = { 814, "POST", "studies/{}/surveys/{}/language/{}", 3 };  // Args: ID, SignalID, Language
      }

      namespace Devices {
        static const WebServiceDetail Types = { 900, "GET", "devices/types", 0 };  
        static const WebServiceDetail RetrieveLicense = { 908, "GET", "devices/license", 0 };  
        static const WebServiceDetail Checkin = { 901, "PATCH", "devices/checkin", 0 };  
        static const WebServiceDetail Retrieve = { 902, "GET", "devices/{}", 1 };  // Args: ID
        static const WebServiceDetail Create = { 903, "POST", "devices", 0 };  
        static const WebServiceDetail Update = { 904, "PATCH", "devices/{}", 1 };  // Args: ID
        static const WebServiceDetail Remove = { 905, "DELETE", "devices/{}", 1 };  // Args: ID
        static const WebServiceDetail List = { 906, "GET", "devices", 0 };  
        static const WebServiceDetail RemoveDeviceMeasurements = { 907, "DELETE", "devices/{}/measurements", 1 };  // Args: ID
      }

      namespace Templates {
        static const WebServiceDetail ListTemplates = { 1100, "GET", "templates", 0 };  
        static const WebServiceDetail ListTypes = { 1101, "GET", "templates/types", 0 };  
        static const WebServiceDetail ListSignals = { 1102, "GET", "templates/signals", 0 };  
        static const WebServiceDetail ListFileTypes = { 1103, "GET", "templates/files", 0 };  
        static const WebServiceDetail CreateTemplate = { 1105, "POST", "templates", 0 };  
        static const WebServiceDetail RetrieveTemplate = { 1104, "GET", "templates/{}", 1 };  // Args: ID
        static const WebServiceDetail DeleteTemplate = { 1106, "DELETE", "templates/{}", 1 };  // Args: ID
        static const WebServiceDetail UpdateTemplate = { 1107, "PATCH", "templates/{}", 1 };  // Args: ID
        static const WebServiceDetail UpdateSignals = { 1108, "PATCH", "templates/{}/signals", 1 };  // Args: ID
        static const WebServiceDetail UpdateFileTypes = { 1109, "PATCH", "templates/{}/files", 1 };  // Args: ID
        static const WebServiceDetail ListConfigFiles = { 1110, "GET", "sdkconfig", 0 };  
        static const WebServiceDetail CreateConfigFile = { 1111, "POST", "sdkconfig", 0 };  
        static const WebServiceDetail DeleteConfigFile = { 1112, "DELETE", "sdkconfig", 0 };  
      }

      namespace OrganizationsPrivate {
        static const WebServiceDetail List = { 1200, "GET", "org-private", 0 };  
        static const WebServiceDetail Create = { 1201, "POST", "org-private", 0 };  
        static const WebServiceDetail Update = { 1202, "PATCH", "org-private/{}", 1 };  // Args: ID
        static const WebServiceDetail TemplateAdd = { 1203, "POST", "org-private/{}/templates", 1 };  // Args: ID
        static const WebServiceDetail TemplateRemove = { 1204, "DELETE", "org-private/{}/templates/{}", 2 };  // Args: ID, TemplateID
        static const WebServiceDetail Retrieve = { 1205, "GET", "org-private/{}", 1 };  // Args: ID
        static const WebServiceDetail TemplateList = { 1206, "GET", "org-private/{}/templates", 1 };  // Args: ID
        static const WebServiceDetail ListAdmins = { 1207, "GET", "org-private/{}/admins", 1 };  // Args: ID
        static const WebServiceDetail CreateAdmin = { 1208, "POST", "org-private/{}/admins", 1 };  // Args: ID
        static const WebServiceDetail RemoveAdmin = { 1209, "DELETE", "org-private/{}/admins/{}", 2 };  // Args: ID, UserID
        static const WebServiceDetail ListUsers = { 1210, "GET", "org-private/{}/users", 1 };  // Args: ID
        static const WebServiceDetail ListStudies = { 1211, "GET", "org-private/{}/studies", 1 };  // Args: ID
        static const WebServiceDetail UpdateLogo = { 1212, "POST", "org-private/{}/logo", 1 };  // Args: ID
        static const WebServiceDetail TemplateUpdate = { 1213, "PATCH", "org-private/{}/templates", 1 };  // Args: ID
        static const WebServiceDetail AddAddress = { 1214, "POST", "org-private/{}/addresses", 1 };  // Args: ID
        static const WebServiceDetail RemoveOrg = { 1215, "DELETE", "org-private/{}", 1 };  // Args: ID
        static const WebServiceDetail CreateConsentForOrg = { 1216, "POST", "org-private/{}/consent", 1 };  // Args: ID
        static const WebServiceDetail MeasurementList = { 1217, "GET", "org-private/{}/measurements", 1 };  // Args: ID
      }

      namespace Reports {
        static const WebServiceDetail ReportReady = { 1300, "POST", "reports/ready", 0 };  
        static const WebServiceDetail ReportCount = { 1301, "POST", "reports/count", 0 };  
        static const WebServiceDetail MakeReport = { 1302, "POST", "reports", 0 };  
      }

      namespace Exports {
        static const WebServiceDetail ExportReadyHandler = { 1300, "POST", "exports/ready", 0 };  
        static const WebServiceDetail ExportCountHandler = { 1301, "POST", "exports/count", 0 };  
        static const WebServiceDetail ExportHandler = { 1302, "POST", "exports", 0 };  
      }

      namespace Licenses {
        static const WebServiceDetail CreateLicenseAllowedStudy = { 1400, "POST", "licenses/studies", 0 };  
        static const WebServiceDetail UpdateLicenseAllowedStudy = { 1401, "PATCH", "licenses/studies/{}", 1 };  // Args: ID
        static const WebServiceDetail RemoveLicenseAllowedStudy = { 1402, "DELETE", "licenses/studies/{}", 1 };  // Args: ID
        static const WebServiceDetail ListLicenseAllowedStudies = { 1403, "GET", "licenses/studies", 0 };  
        static const WebServiceDetail RetrieveLicenseAllowedStudy = { 1404, "GET", "licenses/studies/{}", 1 };  // Args: ID
        static const WebServiceDetail ListLicenses = { 1405, "GET", "licenses", 0 };  
        static const WebServiceDetail ListOrgLicenses = { 1406, "GET", "licenses/organization", 0 };  
        static const WebServiceDetail ListLicenseTypes = { 1412, "GET", "licenses/types", 0 };  
        static const WebServiceDetail CreateLicense = { 1407, "POST", "licenses", 0 };  
        static const WebServiceDetail UpdateLicense = { 1408, "PATCH", "licenses/{}", 1 };  // Args: ID
        static const WebServiceDetail RemoveLicense = { 1409, "DELETE", "licenses/{}", 1 };  // Args: ID
        static const WebServiceDetail RetrieveLicense = { 1410, "GET", "licenses/{}", 1 };  // Args: ID
        static const WebServiceDetail RetrieveOrgLicense = { 1411, "GET", "licenses/organization/{}", 1 };  // Args: ID
      }

      namespace Addresses {
        static const WebServiceDetail Update = { 1700, "PATCH", "addresses/{}", 1 };  // Args: ID
        static const WebServiceDetail Delete = { 1701, "DELETE", "addresses/{}", 1 };  // Args: ID
      }

      namespace Signals {
        static const WebServiceDetail Retrieve = { 1801, "GET", "signals/{}", 1 };  // Args: ID
        static const WebServiceDetail Update = { 1803, "PATCH", "signals/{}", 1 };  // Args: ID
        static const WebServiceDetail RetrieveMultiple = { 1804, "POST", "signals/retrieve-multiple", 0 };  
        static const WebServiceDetail List = { 1805, "GET", "signals", 0 };  
      }

      namespace BillingSchedules {
        static const WebServiceDetail CreateBillingSchedule = { 1900, "POST", "billing/schedules", 0 };  
        static const WebServiceDetail RetrieveBillingSchedule = { 1901, "GET", "billing/schedules/{}", 1 };  // Args: ID
        static const WebServiceDetail RemoveBillingSchedule = { 1901, "DELETE", "billing/schedules/{}", 1 };  // Args: ID
        static const WebServiceDetail UpdateBillingSchedule = { 1903, "PATCH", "billing/schedules/{}", 1 };  // Args: ID
        static const WebServiceDetail ListBillingSchedules = { 1904, "GET", "billing/schedules", 0 };  
      }

      namespace BillingPeriods {
        static const WebServiceDetail CreateBillingPeriod = { 2000, "POST", "billing/periods", 0 };  
        static const WebServiceDetail RetrieveBillingPeriod = { 2001, "GET", "billing/periods/{}", 1 };  // Args: ID
        static const WebServiceDetail RemoveBillingPeriod = { 2001, "DELETE", "billing/periods/{}", 1 };  // Args: ID
        static const WebServiceDetail UpdateBillingPeriod = { 2003, "PATCH", "billing/periods/{}", 1 };  // Args: ID
        static const WebServiceDetail ListBillingPeriods = { 2004, "GET", "billing/schedules/{}/periods", 1 };  // Args: BillingScheduleID
      }

      namespace CostSummaries {
        static const WebServiceDetail CreateCostSummary = { 2100, "POST", "billing/summaries", 0 };  
        static const WebServiceDetail RetrieveCostSummary = { 2101, "GET", "billing/summaries/{}", 1 };  // Args: ID
        static const WebServiceDetail RemoveCostSummary = { 2101, "DELETE", "billing/summaries/{}", 1 };  // Args: ID
        static const WebServiceDetail UpdateCostSummary = { 2103, "PATCH", "billing/summaries/{}", 1 };  // Args: ID
        static const WebServiceDetail ListCostSummaries = { 2104, "GET", "billing/periods/{}/summaries", 1 };  // Args: BillingPeriodID
      }

      namespace Brands {
        static const WebServiceDetail Create = { 2200, "POST", "brands", 0 };  
        static const WebServiceDetail Retrieve = { 2201, "GET", "brands/{}", 1 };  // Args: ID
        static const WebServiceDetail Remove = { 2202, "DELETE", "brands/{}", 1 };  // Args: ID
        static const WebServiceDetail Update = { 2203, "PATCH", "brands/{}", 1 };  // Args: ID
        static const WebServiceDetail List = { 2204, "GET", "brands", 0 };  
        static const WebServiceDetail GetOrganizationBrand = { 2205, "GET", "organizations/{}/brand", 1 };  // Args: OrganizationID
        static const WebServiceDetail RetrieveEmailTemplates = { 2206, "GET", "brands/{}/templates", 1 };  // Args: BrandID
        static const WebServiceDetail CreateEmailTemplate = { 2207, "POST", "brands/{}/templates", 1 };  // Args: BrandID
        static const WebServiceDetail RetrieveEmailTemplate = { 2208, "GET", "brands/templates/{}", 1 };  // Args: ID
        static const WebServiceDetail UpdateEmailTemplate = { 2209, "PATCH", "brands/templates/{}", 1 };  // Args: ID
        static const WebServiceDetail DeleteEmailTemplate = { 2210, "DELETE", "brands/templates/{}", 1 };  // Args: ID
      }

      namespace Auths {
        static const WebServiceDetail SendReset = { 2300, "PATCH", "auths/users/reset", 0 };  
        static const WebServiceDetail ResetPassword = { 2301, "PATCH", "auths/users", 0 };  
        static const WebServiceDetail SendResetMobile = { 2302, "PATCH", "auths/users/reset/mobile", 0 };  
        static const WebServiceDetail RequestLoginCodeV2 = { 2303, "POST", "auths/users/code", 0 };  
        static const WebServiceDetail RenewToken = { 2304, "POST", "auths/renew", 0 };  
        static const WebServiceDetail GenerateToken = { 2305, "POST", "auths/generateToken", 0 };  
      }

    }  // namespace web
  }    // namespace api
}      // namespace dfx

#endif  // DFX_API_WEB_WEBSERVICEDETAIL_H
  