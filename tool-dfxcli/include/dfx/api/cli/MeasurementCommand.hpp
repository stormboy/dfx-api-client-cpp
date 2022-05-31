// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#ifndef DFXAPI_MEASUREMENTCOMMAND_HPP
#define DFXAPI_MEASUREMENTCOMMAND_HPP

#include "dfx/api/cli/DFXAppCommand.hpp"

class MeasurementData;

class MeasurementGetCommand : public DFXAppCommand
{
public:
    MeasurementGetCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result);
    DFXExitCode execute() override;
};

class MeasurementCreateCommand : public DFXAppCommand
{
public:
    MeasurementCreateCommand(CLI::App* app, CLI::App* create, std::shared_ptr<Options> options, DFXExitCode& result);
    DFXExitCode execute() override;

private:
    void writeEventData(const dfx::api::MeasurementResult& result);

    std::string studyID;
    bool disableFileNameSort;
    std::vector<std::string> files;
    unsigned long long delay{0};
    bool verbose;
    bool metrics;
    bool warnings;
    bool noaverage;
    bool streaming;
    std::string outputFile;

    bool needsSaving;
    std::unique_ptr<MeasurementData> measurementData;
};

class MeasurementData
{
public:
    MeasurementData() = default;

    void saveMetricData(const dfx::api::MeasurementMetric& metric);
    void saveWarningData(const dfx::api::MeasurementWarning& warning);
    void saveResultData(const dfx::api::MeasurementResult& result);

    struct ResultData
    {
        std::vector<uint64_t> frameEndTimestamps;
        std::vector<uint64_t> timestamps;
        std::vector<std::vector<float>> data;
    };

    std::vector<float> metrics;
    std::vector<dfx::api::MeasurementWarning> warnings;

    //          FaceID              SignalID
    std::map<std::string, std::map<std::string, ResultData>> resultData;
};

void to_json(nlohmann::json& j, const MeasurementData& d);

#endif // DFXAPI_MEASUREMENTCOMMAND_HPP
