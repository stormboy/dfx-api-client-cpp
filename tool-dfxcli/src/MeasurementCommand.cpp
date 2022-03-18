// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/cli/MeasurementCommand.hpp"
#include "dfx/api/utils/FileUtils.hpp"
#include "nlohmann/json.hpp"

#include "natural_sort/natural_sort.hpp"

#include <filesystem>
#include <thread>

using dfx::api::CloudStatus;
using dfx::api::MeasurementAPI;

using nlohmann::json;
namespace fs = std::filesystem;
using std::filesystem::directory_iterator;

// Windows Filesystem implementation use wide strings
#ifdef _WIN32
#include <codecvt>
#include <locale>
std::string to_string(std::wstring str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    return std::string(utf8_conv.to_bytes(str));
}
#else
std::string to_string(std::string str)
{
    return str;
}
#endif

MeasurementGetCommand::MeasurementGetCommand(CLI::App* get, std::shared_ptr<Options> options, DFXExitCode& result)
    : DFXAppCommand(std::move(options))
{
    cmd = createGetCommand(get, "measurement");
    cmd->callback([&]() { result = cleanup(setupAndExecute()); });
}

DFXExitCode MeasurementGetCommand::execute()
{
    auto measurement = client->measurement(config);
    if (!measurement) {
        CloudStatus status(CLOUD_UNSUPPORTED_TRANSPORT, "Measurement implementation unavailable");
        outputError(status);
        return DFXExitCode::FAILURE;
    }

    // If specified on command line, override the config limit
    if (cmd->count("--limit") > 0) {
        config.listLimit = limit;
    }

    std::unordered_map<MeasurementAPI::MeasurementFilter, std::string> filters;
    if (cmd->count("--full") > 0) {
        filters.emplace(MeasurementAPI::MeasurementFilter::FullObject, "true");
    }

    int16_t totalCount;
    std::vector<dfx::api::Measurement> measurements;
    CloudStatus status(CLOUD_OK);
    if (ids.size() == 0) {
        status = measurement->list(config, filters, offset, measurements, totalCount);
    } else {
        status = measurement->retrieveMultiple(config, ids, measurements);
    }

    if (status.code == CLOUD_OK) {
        outputResponse({{"measurements", measurements}, {"total", totalCount}});
        return DFXExitCode::SUCCESS;
    } else {
        outputError(status);
        return DFXExitCode::FAILURE;
    }
}

MeasurementCreateCommand::MeasurementCreateCommand(CLI::App* app,
                                                   CLI::App* create,
                                                   std::shared_ptr<Options> options,
                                                   DFXExitCode& result)
    : DFXAppCommand(std::move(options)), disableFileNameSort(false), delay(1000), verbose(false), metrics(false),
      warnings(false), needsSaving(false)
{
    if (app == nullptr) {
        cmd = create->add_subcommand("measurement", "Create a new measurement");
    } else {
        cmd = app->add_subcommand("process", "Process measurement payloads");
    }
    cmd->add_option("-d,--delay", delay, "Delay between sent payload files in milliseconds")->capture_default_str();
    cmd->add_option("--study", studyID, "Study ID to use with measurement")->capture_default_str();
    cmd->add_option("-o,--output", outputFile, "Save JSON output to specified filename")->capture_default_str();
    cmd->add_flag("-n,--nosort", disableFileNameSort, "Disable sorting of file names naturally");
    cmd->add_flag("-v,--verbose", verbose, "Verbose information, show warnings and flow")->capture_default_str();
    cmd->add_flag("-m,--metrics", metrics, "Include metrics data in output")->capture_default_str();
    cmd->add_flag("-w,--warnings", warnings, "Include warnings data in output")->capture_default_str();
    cmd->add_option("payloads", files, "Payload files to send, or directory containing payload*.bin")
        ->check(CLI::ExistingPath)
        ->required();
    cmd->callback([&]() { result = cleanup(setupAndExecute()); });
}

DFXExitCode MeasurementCreateCommand::execute()
{
    auto measurementStream = client->measurementStream(config);
    if (!measurementStream) {
        CloudStatus status(CLOUD_UNSUPPORTED_TRANSPORT, "Measurement stream implementation unavailable");
        outputError(status);
        return DFXExitCode::FAILURE;
    }

    // If we are given one name and it is a directory, scan for all the payloads in that folder
    if (files.size() == 1 && std::filesystem::is_directory(files[0])) {
        std::string folder = files[0];
        files.clear();
        for (const auto& file : directory_iterator(folder)) {
            bool hasBinExtension = to_string(file.path().extension().c_str()).compare(".bin") == 0;
            bool startsWithPayload = to_string(file.path().filename().c_str()).rfind("payload", 0) == 0;
            if (file.is_regular_file() && startsWithPayload && hasBinExtension) {
                files.push_back(to_string(file.path()));
            }
        }
    } else {
        int count = 0;
        for (const auto& file : files) {
            if (std::filesystem::is_directory(file)) {
                count++;
            }
        }
        if (count != 0) {
            CloudStatus status(CLOUD_BAD_REQUEST, "Can only specify one directory by itself");
            outputError(status);
            return DFXExitCode::FAILURE;
        }
    }

    if (!disableFileNameSort) {
        SI::natural::sort(files);
    }

    std::string study(config.studyID);
    auto studyOpt = cmd->get_option_no_throw("--study");
    if (studyOpt != nullptr && studyOpt->count() > 0) {
        study = studyID;
    }

    auto outputOpt = cmd->get_option_no_throw("--output");
    if (outputOpt != nullptr && outputOpt->count() > 0) {
        measurementData = std::make_unique<MeasurementData>();
        needsSaving = true;
    }

    if (verbose) {
        metrics = true;
        warnings = true;
    }

    if (metrics) {
        measurementStream->setMetricCallback([&](const dfx::api::MeasurementMetric& metric) {
            if (measurementData != nullptr) {
                measurementData->saveMetricData(metric);
            }

            std::cout << "Metric: " << metric.uploadRate << std::endl;
        });
    }

    if (warnings) {
        measurementStream->setWarningCallback([&](const dfx::api::MeasurementWarning& warning) {
            if (measurementData != nullptr) {
                measurementData->saveWarningData(warning);
            }

            std::cout << "Warning[" << warning.warningCode << "]: " << warning.warningMessage << std::endl;
        });
    }

    measurementStream->setResultCallback([&](const dfx::api::MeasurementResult& result) {
        if (measurementData != nullptr) {
            measurementData->saveResultData(result);
        }

        writeEventData(result);
    });

    addCleanupOperation([this]() {
        if (needsSaving) {
            auto data = static_cast<nlohmann::json>(*measurementData).dump();
            std::vector<uint8_t> vec(data.begin(), data.end());
            if (!dfx::api::utils::writeFile(outputFile, vec)) {
                std::cerr << "Unable to write file: " << outputFile << std::endl;
            }
        }
    });

    auto status = measurementStream->setupStream(config, study);
    if (status.code != CLOUD_OK) {
        outputError(status);
        return DFXExitCode::FAILURE;
    }

    int count = 0;
    for (auto& file : files) {
        const std::vector<uint8_t> chunkData = dfx::api::utils::readFile(file);

        bool isLast = ++count == files.size();
        if (verbose) {
            std::cout << "Sending: " << file << ", isLast? " << (isLast ? "true" : "false") << std::endl;
        }
        status = measurementStream->sendChunk(config, chunkData, isLast);
        if (status.code != CLOUD_OK) {
            std::cerr << static_cast<json>(status) << std::endl;
            return DFXExitCode::FAILURE;
        }

        if (delay > 0) {
            if (verbose) {
                std::cout << "Sleeping for " << delay << " milliseconds" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
    }

    if (verbose) {
        std::cout << "Sending done, waiting for completion" << std::endl;
    }
    measurementStream->waitForCompletion(config);
    if (verbose) {
        std::cout << "Completion received" << std::endl;
    }

    if (measurementData != nullptr) {
        auto data = static_cast<nlohmann::json>(*measurementData).dump();
        std::vector<uint8_t> vec(data.begin(), data.end());
        if (!dfx::api::utils::writeFile(outputFile, vec)) {
            std::cerr << "Unable to write file: " << outputFile << std::endl;
            needsSaving = false;
            return DFXExitCode::FAILURE;
        }
    }

    return DFXExitCode::SUCCESS;
}

void to_json(nlohmann::json& json, const MeasurementData& d)
{
    for (auto& faces : d.resultData) {
        nlohmann::json face;
        for (auto& signals : faces.second) {
            nlohmann::json signal;
            std::vector<float> values;
            bool isScalar = true; // Assume scalar
            for (auto& vec : signals.second.data) {
                if (vec.size() == 1) {
                    values.push_back(vec[0]);
                } else {
                    isScalar = false;
                }
            }
            if (isScalar) {
                signal["data"] = values;
            } else {
                signal["data"] = signals.second.data;
            }
            signal["timestamp"] = signals.second.timestamps;
            signal["frameEndTimestamp"] = signals.second.frameEndTimestamps;
            face[signals.first] = signal;
        }
        json[faces.first] = face;
    }

    if (d.metrics.size() > 0) {
        json["metrics"] = d.metrics;
    }

    if (d.warnings.size() > 0) {
        auto warnings = json::array();
        for (const auto& warning : d.warnings) {
            nlohmann::json item;
            item["timestamp"] = warning.timestampMS;
            item["code"] = warning.warningCode;
            item["message"] = warning.warningMessage;
            warnings.push_back(item);
        }
        json["warnings"] = warnings;
    }
}

void MeasurementCreateCommand::writeEventData(const dfx::api::MeasurementResult& result)
{
    static std::string lastFaceID("");
    static uint64_t lastChunkOrder(0);
    if (lastFaceID.length() == 0 || result.faceID.compare(lastFaceID) != 0 || result.chunkOrder != lastChunkOrder) {
        if (lastFaceID.length() > 0) {
            std::cout << "\n";
        }
        lastFaceID = result.faceID;
        lastChunkOrder = result.chunkOrder;

        std::cout << "Result[" << result.faceID << "," << result.chunkOrder << "] ";
    } else {
        std::cout << ",";
    }
    int numberResults = result.signalData.size();
    for (auto& signal : result.signalData) {
        std::cout << signal.first << "=";
        int numberDataValues = signal.second.size();
        bool scalar = numberDataValues == 1;
        if (!scalar) {
            std::cout << "[";
        }
        for (auto& data : signal.second) {
            std::cout << data;
            if (--numberDataValues > 0) {
                std::cout << ",";
            }
        }
        if (!scalar) {
            std::cout << "]";
        }
        if (--numberResults > 0) {
            std::cout << ",";
        }
    }
}

void MeasurementData::saveMetricData(const dfx::api::MeasurementMetric& metric)
{
    metrics.push_back(metric.uploadRate);
}

void MeasurementData::saveWarningData(const dfx::api::MeasurementWarning& warning)
{
    warnings.push_back(warning);
}

// Save results takes the result stream of events and then builds a data structure
// which is more suitable for JSON output where data is organized around the
// face and signal rather than event timestamp.
void MeasurementData::saveResultData(const dfx::api::MeasurementResult& result)
{
    auto faceIter = resultData.find(result.faceID);
    if (faceIter == resultData.end()) {
        std::map<std::string, ResultData> data;
        resultData[result.faceID] = data;
        faceIter = resultData.find(result.faceID);
    }
    auto& results = (*faceIter).second;
    for (auto& signal : result.signalData) {
        auto signalIter = results.find(signal.first);
        if (signalIter == results.end()) {
            results[signal.first] = ResultData{};
            signalIter = results.find(signal.first);
        }
        auto& signalResultData = signalIter->second;
        signalResultData.frameEndTimestamps.push_back(result.frameEndTimestampMS);
        signalResultData.timestamps.push_back(result.timestampMS);
        signalResultData.data.push_back(signal.second);
    }
}
