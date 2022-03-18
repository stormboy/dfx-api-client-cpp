// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/rest/MeasurementStreamREST.hpp"

#include "fmt/format.h"

using dfx::api::CloudConfig;
using dfx::api::CloudStatus;
using dfx::api::MeasurementStreamAPI;

using namespace dfx::api::rest;

MeasurementStreamREST::MeasurementStreamREST(const CloudConfig& config)
{
    closeMeasurement(CloudStatus(CLOUD_OK));
}

MeasurementStreamREST::~MeasurementStreamREST() {}

CloudStatus MeasurementStreamREST::setupStream(const CloudConfig& config, const std::string& studyID)
{
    return CloudStatus(CLOUD_UNSUPPORTED_FEATURE,
                       fmt::format("{} does not support {} end-point", "REST", "measurement stream"));
}
