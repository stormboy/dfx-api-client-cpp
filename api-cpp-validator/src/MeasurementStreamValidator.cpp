// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/validator/MeasurementStreamValidator.hpp"

#include "CloudValidatorMacros.hpp"

using namespace dfx::api;
using namespace dfx::api::validator;

MeasurementStreamValidator::~MeasurementStreamValidator()
{
    // While the validator isn't a real measurement, the state in the real measurement
    // will assert it was closed
    closeMeasurement(CloudStatus(CLOUD_OK));
}

const MeasurementStreamValidator& MeasurementStreamValidator::instance()
{
    static const MeasurementStreamValidator instance;
    return instance;
}

CloudStatus MeasurementStreamValidator::setupStream(const CloudConfig& config, const std::string& studyID)
{
    return CloudStatus(CLOUD_OK);
}

CloudStatus MeasurementStreamValidator::sendChunk(const CloudConfig& config,
                                                  const std::vector<uint8_t>& chunk,
                                                  bool isLastChunk)
{
    return CloudStatus(CLOUD_OK);
}