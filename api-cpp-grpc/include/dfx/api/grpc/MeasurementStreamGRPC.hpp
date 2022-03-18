// Copyright (c) Nuralogix. All rights reserved. Licensed under the MIT license.
// See LICENSE.txt in the project root for license information.

#pragma once
#ifndef DFX_API_CLOUD_MEASUREMENT_STREAM_GRPC_H
#define DFX_API_CLOUD_MEASUREMENT_STREAM_GRPC_H

#include "dfx/api/MeasurementStreamAPI.hpp"
#include "dfx/measurements/v2/measurements.grpc.pb.h"

#include <condition_variable>
#include <mutex>
#include <thread>

#include <grpcpp/grpcpp.h>
#include <memory>
#include <queue>
#include <string>

// A gRPC Measurement relies on bi-directional streaming which means we can send and receive multiple
// messages, simultaneously. The gRPC ReaderInterface is thread-safe with respect to Write on the same
// stream but should not be performed concurrently with another Read or the order of delivery would not
// be defined.
//
// We leverage the caller's thread to write messages immediately. Since the ReaderInterface blocks during
// the Read() operation we process the reads in a dedicated read thread. Read() will return false when
// no more incoming messages, either because the server called WritesDone() or the stream failed/cancelled.
// At this point, the reader thread is no longer required and terminates.
//
// The client can register a function callback (or lambda) and the reader thread will immediately,
// pass the message to the callback - but the callback needs to be thread safe. Alternatively, if
// no callback is registered this Measurement can be polled for results that are queued as they
// are received.
//
// @see https://grpc.github.io/grpc/cpp/classgrpc_1_1internal_1_1_reader_interface.html

namespace dfx::api::grpc
{

class CloudGRPC;

class MeasurementStreamGRPC : public MeasurementStreamAPI
{
public:
    MeasurementStreamGRPC(const CloudConfig& config, const std::shared_ptr<CloudGRPC>& cloudGRPC);

    ~MeasurementStreamGRPC() override;

    CloudStatus setupStream(const CloudConfig& config, const std::string& studyID) override;

    CloudStatus sendChunk(const CloudConfig& config, const std::vector<uint8_t>& chunk, bool isLastChunk) override;

    CloudStatus cancel(const CloudConfig& config) override;

    CloudStatus reset(const CloudConfig& config) override;

private:
    friend class CloudGRPC;

    void initialize();

    CloudStatus closeStream(CloudStatus status);

    void readerThread(const CloudConfig& config);
    void handleReadResponse(const measurements::v2::StreamResponse& response);
    void sendAsyncRequest(const CloudConfig& config, const measurements::v2::StreamRequest& request);

    const std::chrono::time_point<std::chrono::system_clock> getDeadlineInMs(const unsigned int deadlineMs);

private:
    // recursive so setupStream and reset can call closeStream on failure
    std::recursive_mutex mutex;
    bool writerClosedStream;

    std::mutex mutexMeasurementID;
    std::condition_variable cvMeasurementID;
    std::string measurementID;

    ::grpc::ClientContext clientContext;
    std::shared_ptr<::grpc::Channel> grpcChannel;
    ::grpc::CompletionQueue completionQueue;
    std::unique_ptr<measurements::v2::API::Stub> measurementsStub;
    std::unique_ptr<::grpc::ClientAsyncReaderWriter<measurements::v2::StreamRequest, measurements::v2::StreamResponse>>
        measurementsStream;

    enum GrpcAsyncTag
    {
        StartDone,
        WriteDone,
        ReadDone,
        WritesDone,
        FinishDone
    };
    struct GrpcAsyncTags
    {
        GrpcAsyncTag START_DONE = GrpcAsyncTag::StartDone;
        GrpcAsyncTag WRITE_DONE = GrpcAsyncTag::WriteDone;
        GrpcAsyncTag READ_DONE = GrpcAsyncTag::ReadDone;
        GrpcAsyncTag WRITES_DONE = GrpcAsyncTag::WritesDone;
        GrpcAsyncTag FINISH_DONE = GrpcAsyncTag::FinishDone;
    } tags;

    std::mutex streamMutex;
    std::unique_ptr<std::thread> pReaderThread;
    std::condition_variable cvReaderDone;
    bool readerWasStarted = false;
    bool readerRunning = false;
    bool sendingRequest = false;
    bool readerShouldStop = false;
    std::queue<measurements::v2::StreamRequest> queuedRequests;

    uint16_t chunkOrder;
    bool isFirstChunk;
    bool isLastChunk;
};

} // namespace dfx::api::grpc

#endif // DFX_API_CLOUD_MEASUREMENT_STREAM_GRPC_H
