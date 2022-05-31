// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#include "dfx/api/grpc/MeasurementStreamGRPC.hpp"
#include "dfx/api/grpc/CloudGRPC.hpp"
#include "dfx/api/validator/CloudValidator.hpp"

#include "dfx/action/v1/action.pb.h"

#include "fmt/format.h"
#include <chrono>
#include <google/protobuf/util/time_util.h>
#include <thread>

using dfx::api::CloudAPI;
using dfx::api::CloudConfig;
using dfx::api::CloudStatus;
using dfx::api::MeasurementStreamAPI;
using namespace dfx::api::grpc;

using namespace std::chrono_literals;

// gRPC measurements function on a bi-directional stream but are only good for one measurement.
//
// They expect:
//     1. Upgraded UserToken (containing DeviceToken)
//     2. Study ID in first message
//     3. Payloads to be sent in order
//     4. To be notified of first and last payload
//
// This was re-written to use a CompletionQueue in order to attach a timeout and place
// an upper bound on the socket read blocking. For most of the gRPC calls this is nicely
// handed though the Context timeout, but for a streaming service the Context timeout
// is applied to the duration of the stream and not the message which is not useful.
//
// Normally a CompletionQueue would be used to multiplex many end-points s this nice
// example illustrates:
// https://stackoverflow.com/questions/67784384/c-grpc-clientasyncreaderwriter-how-to-check-if-data-is-available-for-read
// The use of a CompletionQueue is complicated because the request / response happen
// asynchronously... all so it can be assured that the client is able to attach
// a timeout for how long they are willing to wait on a response. Rather than blocking
// the client send thread, the request is queued if the CompletionQueue is busy.

MeasurementStreamGRPC::MeasurementStreamGRPC(const CloudConfig& config, const std::shared_ptr<CloudGRPC>& cloudGRPC)
{
    // Defer channel into setupStream - using it as a flag to indicate active
    initialize();
}

MeasurementStreamGRPC::~MeasurementStreamGRPC()
{
    // The Reader thread is holding a "this" pointer and we have to keep ourselves
    // alive until the thread exits or it's variables will be obliterated. The closeStream()
    // will force gRPC to start a shutdown, which will wake up the gRPC Read() bue we
    // need to wait until thread exits before continuing.
    closeStream(CloudStatus(CLOUD_OK));

    if (readerWasStarted) {
        pReaderThread->join();
    }
}

// Shared with constructor and reset to ensure consistency
void MeasurementStreamGRPC::initialize()
{
    grpcChannel = nullptr;
    measurementsStub = nullptr;
    measurementID = "";
    chunkOrder = 0;
    isFirstChunk = true;
    isLastChunk = false;
    writerClosedStream = false;
}

CloudStatus MeasurementStreamGRPC::setupStream(const CloudConfig& config,
                                               const std::string& studyID,
                                               const std::map<CreateProperty, std::string>& properties)
{
    DFX_CLOUD_VALIDATOR_MACRO(MeasurementStreamValidator, setupStream(config, studyID, properties));

    std::unique_lock<std::recursive_mutex> lock(mutex);

    if (grpcChannel != nullptr) {
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR,
                           fmt::format("stream already created, must call reset before reuse"));
    }

    if (studyID.empty()) {
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, fmt::format("{} is empty", "studyID"));
    }
    if (config.deviceToken.empty()) {
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, fmt::format("{} is empty", "config.deviceToken"));
    }
    if (config.authToken.empty()) {
        return CloudStatus(CLOUD_PARAMETER_VALIDATION_ERROR, fmt::format("{} is empty", "config.userToken"));
    }

    grpcChannel = CloudGRPC::getChannel(config);
    measurementsStub = dfx::measurements::v2::API::NewStub(grpcChannel);

    // gRPC Measurement stream requires a bearer token which is obtained from a DeviceToken
    // which is upgraded to a UserToken. ie. You must first registerDevice, then login
    // to obtain the UserToken credentials necessary to make a call.
    CloudGRPC::setAuthTokenClientContext(config, clientContext, config.authToken);
    // SKIP the deadline on the stream

    measurementsStream = measurementsStub->PrepareAsyncStream(&clientContext, &completionQueue);
    measurementsStream->StartCall(&tags.START_DONE);
    void* got_tag;
    bool ok = false;
    ::grpc::CompletionQueue::NextStatus status =
        completionQueue.AsyncNext(&got_tag, &ok, getDeadlineInMs(config.timeoutMillis));
    switch (status) {
        case ::grpc::CompletionQueue::NextStatus::TIMEOUT:
            return CloudStatus(CLOUD_TIMEOUT, "Timeout on stream setup");
        case ::grpc::CompletionQueue::NextStatus::GOT_EVENT:
            break;
        case ::grpc::CompletionQueue::NextStatus::SHUTDOWN:
            return CloudStatus(CLOUD_TRANSPORT_CLOSED, "Shutdown on stream setup");
    }
    assert(status == ::grpc::CompletionQueue::NextStatus::GOT_EVENT && ok == 1 &&
           *(static_cast<int*>(got_tag)) == tags.START_DONE);

    // Opening a new measurement, we need to tell it the study_id to use on our first request.
    dfx::measurements::v2::StreamRequest request;
    auto pSetting = request.mutable_setting();
    pSetting->set_study_id(studyID);

    measurementsStream->Write(request, &tags.WRITE_DONE);

    ok = false;
    status = completionQueue.AsyncNext(&got_tag, &ok, getDeadlineInMs(config.timeoutMillis));
    switch (status) {
        case ::grpc::CompletionQueue::NextStatus::TIMEOUT:
            return CloudStatus(CLOUD_TIMEOUT, "Timeout on sending study id");
        case ::grpc::CompletionQueue::NextStatus::GOT_EVENT:
            // The studyID write acknowledgement... the only thing it can be
            break;
        case ::grpc::CompletionQueue::NextStatus::SHUTDOWN:
            return CloudStatus(CLOUD_TRANSPORT_CLOSED, "Shutdown on stream during study initialization");
    }

    if (status == ::grpc::CompletionQueue::NextStatus::GOT_EVENT) {
        // Sometimes, unreliably, seen strings in the debug_error_string... this check
        // might not be doing anything but hoping it caches some early closures before
        // we go to the effort of starting up a thread.
        bool stillValid = clientContext.debug_error_string().empty();
        if (stillValid) {
            // It appears we can communicate, start our thread to handle server responses.
            pReaderThread = std::make_unique<std::thread>(&MeasurementStreamGRPC::readerThread, this, config);
            return CloudStatus(CLOUD_OK);
        }
    }

    return closeStream(CloudStatus(CLOUD_INTERNAL_ERROR, "Received unexpected event"));
}

CloudStatus MeasurementStreamGRPC::closeStream(CloudStatus status)
{
    // Hold lock for duration of method, we don't want multiple threads to attempt to close
    std::unique_lock<std::recursive_mutex> lock(mutex);

    if (isMeasurementClosed(status)) {
        return status; // if it has already been closed, nothing left to do
    }

    // We won't be writing anymore, the connection appears to be closing.

    // Wait for reader thread to terminate sequence...
    if (readerWasStarted && readerRunning) {
        std::unique_lock<std::mutex> lock(streamMutex);
        readerShouldStop = true;
        cvReaderDone.wait(lock); // Nope, need to wait until we get notified
    }

    status = closeMeasurement(status);

    {
        // Stream is being closed, don't let any sendChunk threads waiting on a
        // measurement ID wait any longer, this stream is being closed.
        //
        // Needs to occur AFTER closeMeasurement to ensure the awoken thread
        // will return the proper status value to calling client
        std::unique_lock<std::mutex> lock(mutexMeasurementID);
        cvMeasurementID.notify_all();
    }

    return status;
}

CloudStatus MeasurementStreamGRPC::reset(const CloudConfig& config)
{
    std::unique_lock<std::recursive_mutex> lock(mutex);

    // Notify server if we haven't already
    cancel(config);

    // Ensure the current thread and stream is cleaned up
    auto status = closeStream(CloudStatus(CLOUD_OK));

    // Reset the state back to the constructed state
    initialize();

    return status;
}

const std::chrono::time_point<std::chrono::system_clock>
MeasurementStreamGRPC::getDeadlineInMs(const unsigned int deadlineMs)
{
    const std::chrono::time_point<std::chrono::system_clock> deadline =
        std::chrono::system_clock::now() + std::chrono::milliseconds(deadlineMs);
    return deadline;
}

void MeasurementStreamGRPC::readerThread(const CloudConfig& config)
{
    {
        std::unique_lock<std::recursive_mutex> lock(mutex);
        readerWasStarted = true;
        readerRunning = true;
    }

    dfx::measurements::v2::StreamResponse response;
    ::grpc::CompletionQueue::NextStatus status;
    ::grpc::Status grpcStatus;

    measurementsStream->Read(&response, &tags.READ_DONE);

    CloudStatus cloudStatus(CLOUD_OK);
    bool shutdown = false;
    bool writesDoneSent = false;
    bool finishDoneSent = false;
    while (!shutdown) {
        void* receivedTag;
        bool ok = false;

        status = completionQueue.AsyncNext(&receivedTag, &ok, getDeadlineInMs(config.timeoutMillis));

        GrpcAsyncTag* tag = reinterpret_cast<GrpcAsyncTag*>(receivedTag);
        switch (status) {
            case ::grpc::CompletionQueue::NextStatus::TIMEOUT:
                if (cloudStatus.OK()) {
                    cloudStatus = CloudStatus(CLOUD_TIMEOUT, "Try increasing CloudConfig timeout");
                }
                if (!writesDoneSent) { // Tell the server we are done
                    writesDoneSent = true;
                    measurementsStream->WritesDone(&tags.WRITES_DONE);
                }
                break;
            case ::grpc::CompletionQueue::NextStatus::GOT_EVENT:
                break;
            case ::grpc::CompletionQueue::NextStatus::SHUTDOWN:
                // Not sure the circumstance of this event, difficult to simulate
                cloudStatus = CloudStatus(CLOUD_TRANSPORT_CLOSED, "Received shutdown event");
                shutdown = true;
                continue;
        }

        // We can get an event which is not ok, and still get subsequent events. To be a nice
        // client we need to notify the server we are done writing and want to finish.
        if (!ok) {
            // We are done, get the final status
            if (!writesDoneSent) { // Tell the server we are done
                writesDoneSent = true;
                measurementsStream->WritesDone(&tags.WRITES_DONE);
            } else {
                if (!finishDoneSent) {
                    finishDoneSent = true;
                    measurementsStream->Finish(&grpcStatus, &tags.FINISH_DONE);
                }
            }
            continue;
        }

        switch (*tag) {
            case GrpcAsyncTag::ReadDone:
                handleReadResponse(response);
                measurementsStream->Read(&response, &tags.READ_DONE);
                break;
            case GrpcAsyncTag::WriteDone: {
                if (!writesDoneSent) { // If we have already sent the writesDone event, do not write anything more
                    std::lock_guard lock(streamMutex);
                    if (!queuedRequests.empty()) {
                        measurementsStream->Write(queuedRequests.front(), &tags.WRITE_DONE);
                        queuedRequests.pop();
                    } else {
                        sendingRequest = false;
                        if (isLastChunk) {
                            // This was our last chunk, tell the server we are done writing.
                            writesDoneSent = true;
                            measurementsStream->WritesDone(&tags.WRITES_DONE);
                        }
                    }
                }
                break;
            }
            case GrpcAsyncTag::WritesDone: {
                // If this was our last, hint to the server we are done writing.
                std::unique_lock<std::recursive_mutex> lock(mutex);
                writerClosedStream = true;
                break;
            }
            case GrpcAsyncTag::StartDone:
                assert(*tag != GrpcAsyncTag::StartDone); // Unexpected START Event
                break;
            case GrpcAsyncTag::FinishDone:
                if (cloudStatus.OK()) { // If we were still in good standing, give grpc stream a chance to weigh in
                    // This can return bad Study ID, possibly others?
                    cloudStatus = CloudGRPC::translateGrpcStatus(grpcStatus);
                }
                shutdown = true;
                break;
            default:
                assert(false); // Unexpected event
        }
    }

    // If we have reached here - the server is done - quit allowing any more messages and shut
    // the stream down.
    {
        std::unique_lock<std::recursive_mutex> lock(mutex);
        readerRunning = false;
    }

    closeStream(cloudStatus);

    // If the sender thread initiated a close, it will block waiting for this thread to terminate. Notify it
    // that the thread is now terminated and it can continue the shutdown.
    std::unique_lock<std::mutex> lock(streamMutex);
    cvReaderDone.notify_all();
}

CloudStatus MeasurementStreamGRPC::sendChunk(const CloudConfig& config,
                                             const std::vector<uint8_t>& chunk,
                                             bool isLastChunk)
{
    CloudStatus status(CLOUD_OK);

    // Need to check before waiting on condition, reader thread may never set
    if (isMeasurementClosed(status)) {
        return status; // if it has already been closed.
    }

    // We need a measurement_id to fill into the session_id property here.
    // Yes, that makes zero sense to me too but that is what is required.
    //
    // When the stream was setup, a request to get a measurement_id was
    // performed and so we likely have one but depending on server load
    // and how quick we follow with a sendChunk we may need to block to
    // await a measurement_id to continue with this request.
    std::string measurementID;
    {
        std::unique_lock<std::mutex> lock(mutexMeasurementID);
        if (this->measurementID.empty()) { // Did we get anything back yet?
            cvMeasurementID.wait(lock);    // Nope, need to wait until we get notified

            // If we got woke up, we should have a measurement ID or the stream has been closed
            if (isMeasurementClosed(status)) {
                return status; // if it has already been closed.
            }
            assert(!this->measurementID.empty());
        }
        measurementID = this->measurementID;
    }

    dfx::measurements::v2::StreamRequest request;

    dfx::action::v1::PayloadAction action(dfx::action::v1::PayloadAction::PROCESS);
    if (isFirstChunk) {
        action = dfx::action::v1::PayloadAction::FIRST;
        isFirstChunk = false; // No longer our first request
    }
    if (isLastChunk) { // If this is the last, let the server know (more important then knowing it is first)
        action = dfx::action::v1::PayloadAction::LAST;
        this->isLastChunk = isLastChunk;
    }

    auto pChunk = request.mutable_chunk();
    pChunk->set_action(action);
    pChunk->set_chunk_order(chunkOrder++); // Server expects sequential ordering
    pChunk->set_session_id(measurementID);
    pChunk->set_payload(std::string(chunk.begin(), chunk.end())); // gRPC uses strings for byte arrays

    sendAsyncRequest(config, request);

    return CloudStatus(CLOUD_OK);
}

CloudStatus MeasurementStreamGRPC::cancel(const CloudConfig& config)
{
    CloudStatus status(CLOUD_OK);
    std::unique_lock<std::recursive_mutex> lock(mutex);

    // If measurement has already been closed, we don't want to attempt a cancel on a bad stream
    if (isMeasurementClosed(status)) {
        return status;
    }

    // Best effort... out-of-band signal if we are not already closed
    clientContext.TryCancel();

    return status;
}

void MeasurementStreamGRPC::sendAsyncRequest(const CloudConfig& config,
                                             const dfx::measurements::v2::StreamRequest& request)
{
    std::lock_guard lock(streamMutex);
    if (!sendingRequest) {
        sendingRequest = true;
        measurementsStream->Write(request, &tags.WRITE_DONE);
    } else {
        queuedRequests.push(request);
    }
}

void MeasurementStreamGRPC::handleReadResponse(const dfx::measurements::v2::StreamResponse& response)
{
    int64_t timestampMilliseconds = 0;

    if (response.has_submitted()) {
        timestampMilliseconds = google::protobuf::util::TimeUtil::TimestampToMilliseconds(response.submitted());
    }

    // We got a response, but what type of response is it?
    if (response.has_setting()) {
        {
            std::unique_lock<std::mutex> lock(mutexMeasurementID);
            measurementID = response.setting().measurement_id();
            cvMeasurementID.notify_all(); // Wakeup the sendChunk thread if they are waiting on an ID
        }
        handleMeasurementID(measurementID);
    } else if (response.has_chunk_result()) {
        const auto& chunkResult = response.chunk_result();

        MeasurementResult data;
        data.timestampMS = timestampMilliseconds;
        data.faceID = chunkResult.face_id();

        data.frameEndTimestampMS =
            google::protobuf::util::TimeUtil::TimestampToMilliseconds(chunkResult.frame_end_time());
        data.chunkOrder = chunkResult.chunk_order();

        for (const auto& signal : chunkResult.signal_group_results()) {
            std::string name = signal.signal_name();

            std::vector<float> floatData;
            floatData.reserve(signal.data().size());
            std::copy(signal.data().begin(), signal.data().end(), std::back_inserter(floatData));

            data.signalData.emplace(name, floatData);
        }

        handleResult(data);
    } else if (response.has_metric()) {
        MeasurementMetric metric;
        metric.uploadRate = response.metric().upload_rate();
        handleMetric(metric);
    } else if (response.has_internal_error()) {
        // Does NOT terminate the stream but provides feedback to client
        MeasurementWarning warning;
        warning.timestampMS = timestampMilliseconds;
        warning.warningMessage = response.internal_error().message();
        warning.warningCode = static_cast<int>(response.internal_error().code());
        handleWarning(warning);
    }
}