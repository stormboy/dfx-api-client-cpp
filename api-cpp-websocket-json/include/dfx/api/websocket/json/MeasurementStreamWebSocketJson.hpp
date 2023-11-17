// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_MEASUREMENT_STREAM_WEBSOCKET_JSON_H
#define DFX_API_CLOUD_MEASUREMENT_STREAM_WEBSOCKET_JSON_H

#include "dfx/api/MeasurementStreamAPI.hpp"
#include "dfx/proto/measurements.pb.h"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace dfx::api::websocket::json
{

class CloudWebSocketJson;

class MeasurementStreamWebSocketJson : public MeasurementStreamAPI
{
public:
    MeasurementStreamWebSocketJson(const CloudConfig& config,
                                   const std::shared_ptr<CloudWebSocketJson>& cloudWebSocketJson);

    ~MeasurementStreamWebSocketJson() override;

    CloudStatus sendChunk(const CloudConfig& config, const std::vector<uint8_t>& chunk, bool isLast) override;

    CloudStatus cancel(const CloudConfig& config) override;

private:
    friend class CloudWebSocketJson;

    void initialize();

    CloudStatus setupStream(const CloudConfig& config,
                            const std::string& studyID,
                            const std::map<CreateProperty, std::string>& properties = {}) override;

    void handleStreamResponse(const std::shared_ptr<std::vector<uint8_t>>& message);

    CloudStatus closeStream();

private:
    // recursive so setupStream and reset can call closeStream on failure
    std::recursive_mutex mutex;
    bool streamOpen;
    bool writerClosedStream;
    bool lastChunkSent;

    std::mutex mutexMeasurementID;
    std::condition_variable cvMeasurementID;
    std::string measurementID;
    std::string requestID;

    std::atomic<int> chunksOutstanding;

    uint16_t chunkOrder;
    bool isFirstChunk;

private:
    std::shared_ptr<CloudWebSocketJson> cloudWebSocketJson;
};

} // namespace dfx::api::websocket::json

#endif // DFX_API_CLOUD_MEASUREMENT_STREAM_WEBSOCKET_JSON_H
