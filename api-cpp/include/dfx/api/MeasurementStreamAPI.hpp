// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_MEASUREMENT_STREAM_API_H
#define DFX_API_CLOUD_MEASUREMENT_STREAM_API_H

#include "dfx/api/CloudAPI_Export.hpp"
#include "dfx/api/CloudConfig.hpp"
#include "dfx/api/CloudStatus.hpp"

#include <condition_variable>
#include <cstdint>
#include <deque>
#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace dfx::api
{

class CloudAPI;

/**
 * @brief MeasurementResult is the message type of result data from the server.
 */
struct DFXCLOUD_EXPORT MeasurementResult
{
    uint64_t chunkOrder; ///< The Chunk Order
    std::string faceID;  ///< The Face ID
    std::map<std::string, std::vector<float>> signalData;
    int64_t frameEndTimestampMS;
    int64_t timestampMS;
};

struct DFXCLOUD_EXPORT MeasurementMetric
{
    float uploadRate;
};

struct DFXCLOUD_EXPORT MeasurementWarning
{
    int warningCode;
    std::string warningMessage;
    int64_t timestampMS;
};

/**
 * @brief Asynchronous callback signature to receive a Measurement ID.
 *
 * Provides the Measurement ID being used from this Measurement instance.
 */
typedef std::function<void(const std::string& measurementID)> MeasurementIDCallback;

/**
 * @brief Asynchronous callback signature to receive Measurement results.
 *
 * Provides the actual measurement results from the server for individual
 * signals like heart rate, or signal-to-noise ratio.
 */
typedef std::function<void(const MeasurementResult& result)> MeasurementResultCallback;

/**
 * @brief Asynchronous callback signature to receive Measurement metrics.
 *
 * Provides diagnostic information on the connection speed.
 */
typedef std::function<void(const MeasurementMetric& result)> MeasurementMetricCallback;

/**
 * @brief Asynchronous callback signature to receive Measurement warnings.
 *
 * The warning Callback is used on gRPC streams to provide warning notifications when
 * a signal can't be provided because a criteria has not been met. The connection has not been
 * terminated and the server will continue to attempt to process but it is advice that would
 * improve the signals being provided.
 */
typedef std::function<void(const MeasurementWarning& warning)> MeasurementWarningCallback;

/**
 * @brief Measurement is used to send payload chunks to the DFX Server and get back results.
 *
 * The sendChunk operation is used to provide the server with payload chunks which the
 * server will process and reply with result data. Because of the latency involved in
 * the network communication, uploading, processing and result return this class has
 * been designed to work on a background thread and offers two forms of use.
 *
 * If you prefer asynchronous results, you can register callbacks of the message
 * types you are interested in and you will be called immediately when there are
 * results. The callback itself needs to be thread-safe. If there are any queued
 * messages, those will be delivered the instant a callback is registered and the
 * queue will be cleared.
 *
 * If you prefer synchronous results, you can poll the Measurement for results
 * using the getResult (and associated signatures) with an optional timeout. If
 * you provide no timeout, it will wait until a response is received which might
 * be when the underlying connection itself dies.
 */
class DFXCLOUD_EXPORT MeasurementStreamAPI
{
public:
    /**
     * @brief MeasurementStreamAPI constructor.
     */
    MeasurementStreamAPI();

    /**
     * @brief Measurement destructor.
     */
    virtual ~MeasurementStreamAPI();

    /**
     * @brief Setup the stream.
     *
     * @param config
     * @param studyID
     * @return status of operation, CLOUD_OK on SUCCESS
     */
    virtual CloudStatus setupStream(const CloudConfig& config, const std::string& studyID);

    /**
     * @brief Asynchronously send a payload chunk to the server for processing.
     *
     * @param config the connection configuration to use when sending the chunk.
     * @param chunk the payload chunk of bytes obtained from DFX SDK.
     * @param isLastChunk flag indicating if this is the last chunk for proper measurement completion.
     * @return status of the measurement connection, CLOUD_OK on SUCCESS
     */
    virtual CloudStatus sendChunk(const CloudConfig& config, const std::vector<uint8_t>& chunk, bool isLastChunk);

    /**
     * @brief Waits for the measurement connection to close ensuring that all results
     * have been properly received.
     *
     * In order for the server to properly close the connection when it has completed
     * processing all the chunks, the last chunk needs to be flagged on the sendChunk
     * call or this wait for completion may wait a very long time.
     *
     * @param config the connection configuration to use when waiting.
     * @param timeoutMillis the amount of time to wait for completion, zero is wait forever.
     * @return status of the measurement connection at close, CLOUD_OK on SUCCESS
     */
    virtual CloudStatus waitForCompletion(const CloudConfig& config, int32_t timeoutMillis = 0);

    /**
     * @brief cancel will inform the server that the Measurement should be terminated.
     *
     * This will notify the server of the intent to cancel but leave all internal
     * state setup to receive any final messages the server might wish to deliver.
     *
     * @param config the connection configuration to use when cancelling.
     * @return status of operation, CLOUD_OK on SUCCESS
     */
    virtual CloudStatus cancel(const CloudConfig& config);

    /**
     * @brief Resets this measurement back so another stream can be setup.
     *
     * The current measurement will be cancelled and the state immediately cleared
     * so any undelivered messages would be lost, but the instance will be
     * able to perform another setupStream() without having to be entirely
     * recreated.
     *
     * @param config the connection configuration to use when cancelling.
     * @return status of operation, CLOUD_OK on SUCCESS
     */
    virtual CloudStatus reset(const CloudConfig& config);

    /**
     * @brief Register an asynchronous callback for receiving the Measurement ID.
     *
     * @param callback the callback to invoke when a Measurement ID is available.
     * @return status of operation, CLOUD_OK on SUCCESS
     */
    virtual CloudStatus setMeasurementIDCallback(const MeasurementIDCallback& callback);

    /**
     * @brief Register an asynchronous callback for receiving results.
     *
     * @param callback the callback to invoke when a result is available.
     * @return status of operation, CLOUD_OK on SUCCESS
     */
    virtual CloudStatus setResultCallback(const MeasurementResultCallback& callback);

    /**
     * @brief Register an asynchronous callback for receiving metrics.
     *
     * @param callback the callback to invoke when a metric is available.
     * @return status of operation, CLOUD_OK on SUCCESS
     */
    virtual CloudStatus setMetricCallback(const MeasurementMetricCallback& callback);

    /**
     * @brief Register an asynchronous callback for receiving warnings.
     *
     * @param callback the callback to invoke when a warning message is available.
     * @return status of operation, CLOUD_OK on SUCCESS
     */
    virtual CloudStatus setWarningCallback(const MeasurementWarningCallback& callback);

    /**
     * @brief Synchronously poll for a Measurement ID until timeout expires or connection
     * dies.
     *
     * @param measurementID the measurement ID if the CloudStatus is CLOUD_OK.
     * @param timeoutMillis the amount of time to wait for value, zero is wait forever.
     * @return CLOUD_OK on success, CLOUD_TIMEOUT on timeout or another error status if
     * measurement has been terminated.
     */
    virtual CloudStatus getMeasurementID(std::string& measurementID, int32_t timeoutMillis = 0);

    /**
     * @brief Synchronously poll for a Measurement Result until timeout expires or
     * connection dies.
     *
     * @param result a measurement result if the CloudStatus is CLOUD_OK.
     * @param timeoutMillis the amount of time to wait for value, zero is wait forever.
     * @return CLOUD_OK on success, CLOUD_TIMEOUT on timeout or another error status if
     * measurement has been terminated.
     */
    virtual CloudStatus getResult(MeasurementResult& result, int32_t timeoutMillis = 0);

    /**
     * @brief Synchronously poll for a Measurement Metric until timeout expires or
     * connection dies.
     *
     * @param result a measurement metric if the CloudStatus is CLOUD_OK.
     * @param timeoutMillis the amount of time to wait for value, zero is wait forever.
     * @return CLOUD_OK on success, CLOUD_TIMEOUT on timeout or another error status if
     * measurement has been terminated.
     */
    virtual CloudStatus getMetric(MeasurementMetric& metric, int32_t timeoutMillis = 0);

    /**
     * @brief Synchronously poll for a Measurement Warning until timeout expires or
     * connection dies.
     *
     * @param result a measurement warning if the CloudStatus is CLOUD_OK.
     * @param timeoutMillis the amount of time to wait for value, zero is wait forever.
     * @return CLOUD_OK on success, CLOUD_TIMEOUT on timeout or another error status if
     * measurement has been terminated.
     */
    virtual CloudStatus getWarning(MeasurementWarning& warning, int32_t timeoutMillis = 0);

protected:
    /**
     * @brief handleMeasurementID is called by derived implementations when they
     * receive a measurement ID.
     *
     * @param measurementID the measurementID received.
     * @return status of operation, CLOUD_OK on SUCCESS
     */
    CloudStatus handleMeasurementID(const std::string& measurementID);

    /**
     * @brief handleResult is called by derived implementations when they
     * receive a measurement result.
     *
     * @param result the measurement result received.
     * @return status of operation, CLOUD_OK on SUCCESS
     */
    CloudStatus handleResult(const MeasurementResult& result);

    /**
     * @brief handleMetric is called by derived implementations when they
     * receive a measurement metric.
     *
     * @param metric the measurement metric received.
     * @return status of operation, CLOUD_OK on SUCCESS
     */
    CloudStatus handleMetric(const MeasurementMetric& metric);

    /**
     * @brief handleWarning is called by derived implementations when they
     * receive a measurement warning.
     *
     * @param warning the measurement warning received.
     * @return status of operation, CLOUD_OK on SUCCESS
     */
    CloudStatus handleWarning(const MeasurementWarning& warning);

    /**
     * @brief isMeasurementClosed is called by derived implementations when
     * they are interested if the measurement has been closed.
     *
     * @param status of the closed connection, if it is closed.
     * @return true if the connection has been closed, false if it still active.
     */
    bool isMeasurementClosed(CloudStatus& status);

    /**
     * @brief closeMeasurement is called by derived implementations when
     * they need to ensure the measurement is closed, either the connection
     * has died or the measurement has processed it's last chunk.
     *
     * @param status of the closed connection that the derived implementation would like to use.
     * @return status of the closed connection that was used.
     */
    CloudStatus closeMeasurement(const CloudStatus& status);

private:
    std::shared_ptr<CloudAPI> cloudAPI;

    /**
     * @brief waitForQueuedData is an internal method which holds the implementation for how
     * all the various message types are handled when a get request is performed.
     *
     * @tparam T the message type.
     * @param condition the condition variable protecting the message type.
     * @param timeoutMillis the number of milliseconds, or zero for infinite that should be waited.
     * @param queue the queue for message type holding any queued values.
     * @param result the value obtained after waiting.
     * @return CLOUD_OK on success, CLOUD_TIMEOUT on timeout or another error if connection
     * has been closed.
     */
    template <typename T>
    CloudStatus
    waitForQueuedData(std::condition_variable& condition, int32_t timeoutMillis, std::deque<T>& queue, T& result);

    template <typename T, typename F>
    CloudStatus setCallbackVariable(F& variableCallback, std::deque<T>& queue, const F& callback);

    template <typename T, typename F>
    CloudStatus handle(std::condition_variable& condition, std::deque<T>& queue, const F& callback, const T& result);

    std::mutex measurementMutex;
    std::condition_variable cvWaitForCompletion;
    bool measurementClosed;
    CloudStatus measurementStatus;

    MeasurementIDCallback measurementIDCallback;
    std::condition_variable cvWaitForMeasurementID;
    std::deque<std::string> measurementIDs;

    MeasurementResultCallback resultCallback;
    std::condition_variable cvWaitForResults;
    std::deque<MeasurementResult> measurementResults;

    MeasurementMetricCallback metricCallback;
    std::condition_variable cvWaitForMetrics;
    std::deque<MeasurementMetric> measurementMetrics;

    MeasurementWarningCallback warningCallback;
    std::condition_variable cvWaitForWarnings;
    std::deque<MeasurementWarning> measurementWarnings;
};

} // namespace dfx::api

#endif // DFX_API_CLOUD_MEASUREMENT_STREAM_API_H
