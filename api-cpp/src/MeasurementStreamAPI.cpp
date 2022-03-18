// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/MeasurementStreamAPI.hpp"

#include <chrono>

using namespace dfx::api;

using namespace std::chrono;
using namespace std::chrono_literals;

MeasurementStreamAPI::MeasurementStreamAPI() : measurementClosed(false), measurementStatus(CLOUD_OK) {}

MeasurementStreamAPI::~MeasurementStreamAPI()
{
    // Implementations should have closed prior to this point
    assert(measurementClosed);
}

CloudStatus MeasurementStreamAPI::setupStream(const CloudConfig& config, const std::string& studyID)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus MeasurementStreamAPI::sendChunk(const CloudConfig& config,
                                            const std::vector<uint8_t>& chunk,
                                            bool isLastChunk)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus MeasurementStreamAPI::cancel(const CloudConfig& config)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

CloudStatus MeasurementStreamAPI::reset(const CloudConfig& config)
{
    return CloudStatus(CLOUD_UNIMPLEMENTED_FEATURE);
}

template <typename T, typename F>
CloudStatus MeasurementStreamAPI::setCallbackVariable(F& variableCallback, std::deque<T>& queue, const F& callback)
{
    std::lock_guard<std::mutex> lock(measurementMutex);
    variableCallback = callback;
    if (variableCallback) {
        for (const auto& result : queue) {
            variableCallback(result);
        }
        queue.clear();
    }
    return measurementStatus;
}

[[maybe_unused]] CloudStatus MeasurementStreamAPI::setMeasurementIDCallback(const MeasurementIDCallback& callback)
{
    return setCallbackVariable(measurementIDCallback, measurementIDs, callback);
}

CloudStatus MeasurementStreamAPI::setResultCallback(const MeasurementResultCallback& callback)
{
    return setCallbackVariable(resultCallback, measurementResults, callback);
}

CloudStatus MeasurementStreamAPI::setMetricCallback(const MeasurementMetricCallback& callback)
{
    return setCallbackVariable(metricCallback, measurementMetrics, callback);
}

CloudStatus MeasurementStreamAPI::setWarningCallback(const MeasurementWarningCallback& callback)
{
    return setCallbackVariable(warningCallback, measurementWarnings, callback);
}

template <typename T>
CloudStatus MeasurementStreamAPI::waitForQueuedData(std::condition_variable& condition,
                                                    int32_t timeoutMillis,
                                                    std::deque<T>& queue,
                                                    T& result)
{
    std::unique_lock<std::mutex> lock(measurementMutex);
    long long int timeLeftMilliseconds = timeoutMillis;
    while (true) {
        // If our timer has elapsed, and we have nothing... return
        if (timeLeftMilliseconds <= 0 && queue.empty()) {
            return CloudStatus(CLOUD_TIMEOUT);
        }

        // If we have something... return it
        if (!queue.empty()) {
            result = queue.front();
            queue.pop_front();
            return CloudStatus(CLOUD_OK);
        }

        // Since multiple threads could be sitting on this conditional, when we are notified we
        // may not actually "get" the item and so have to go back to sleep again which is fine
        // so long as we don't exceed the client's requested wait period.
        auto beforeWaitMilliseconds = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

        // NOLINTNEXTLINE(bugprone-spuriously-wake-up-functions)
        if (std::cv_status::timeout == condition.wait_for(lock, timeLeftMilliseconds * 1ms)) {
            return CloudStatus(CLOUD_TIMEOUT); // We timed out waiting to be notified
        } else {
            auto afterWaitMilliseconds = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            auto consumedMilliseconds = afterWaitMilliseconds - beforeWaitMilliseconds;
            timeLeftMilliseconds -= consumedMilliseconds;
        }
    }
}

CloudStatus MeasurementStreamAPI::getMeasurementID(std::string& result, int32_t timeoutMillis)
{
    return waitForQueuedData(cvWaitForMeasurementID, timeoutMillis, measurementIDs, result);
}

CloudStatus MeasurementStreamAPI::getResult(MeasurementResult& result, int32_t timeoutMillis)
{
    return waitForQueuedData(cvWaitForResults, timeoutMillis, measurementResults, result);
}

CloudStatus MeasurementStreamAPI::getMetric(MeasurementMetric& result, int32_t timeoutMillis)
{
    return waitForQueuedData(cvWaitForMetrics, timeoutMillis, measurementMetrics, result);
}

CloudStatus MeasurementStreamAPI::getWarning(MeasurementWarning& result, int32_t timeoutMillis)
{
    return waitForQueuedData(cvWaitForWarnings, timeoutMillis, measurementWarnings, result);
}

template <typename T, typename F>
CloudStatus MeasurementStreamAPI::handle(std::condition_variable& condition,
                                         std::deque<T>& queue,
                                         const F& callback,
                                         const T& result)
{
    std::lock_guard<std::mutex> lock(measurementMutex);
    if (callback) {
        callback(result);
    } else {
        queue.push_back(result);
        condition.notify_all();
    }
    return measurementStatus;
}

CloudStatus MeasurementStreamAPI::handleMeasurementID(const std::string& measurementID)
{
    return handle(cvWaitForMeasurementID, measurementIDs, measurementIDCallback, measurementID);
}

CloudStatus MeasurementStreamAPI::handleResult(const MeasurementResult& result)
{
    return handle(cvWaitForResults, measurementResults, resultCallback, result);
}

CloudStatus MeasurementStreamAPI::handleMetric(const MeasurementMetric& metric)
{
    return handle(cvWaitForMetrics, measurementMetrics, metricCallback, metric);
}

CloudStatus MeasurementStreamAPI::handleWarning(const MeasurementWarning& warning)
{
    return handle(cvWaitForWarnings, measurementWarnings, warningCallback, warning);
}

CloudStatus MeasurementStreamAPI::waitForCompletion(const CloudConfig& config, int32_t timeoutMillis)
{
    std::unique_lock<std::mutex> lock(measurementMutex);
    if (!measurementClosed) { // Need to check before waiting on condition, reader thread may never set
        if (timeoutMillis >= 0) {
            cvWaitForCompletion.wait(lock); // User wants to wait until we get notified
        } else {
            if (std::cv_status::timeout == cvWaitForCompletion.wait_for(lock, timeoutMillis * 1ms)) {
                return CloudStatus(CLOUD_TIMEOUT); // We timed out waiting to be notified
            }
        }
    }
    return measurementStatus;
}

bool MeasurementStreamAPI::isMeasurementClosed(CloudStatus& status)
{
    std::unique_lock<std::mutex> lock(measurementMutex);
    if (measurementClosed) {
        status = measurementStatus;
    }
    return measurementClosed;
}

CloudStatus MeasurementStreamAPI::closeMeasurement(const CloudStatus& status)
{
    std::unique_lock<std::mutex> lock(measurementMutex);

    // If anyone was waiting on this, it has now happened.
    cvWaitForCompletion.notify_all();

    measurementClosed = true;
    measurementStatus = status;

    return measurementStatus;
}
