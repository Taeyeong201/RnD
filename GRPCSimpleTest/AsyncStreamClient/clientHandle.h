#pragma once
#include <iostream>
#include <memory>
#include <fstream>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include "stream.grpc.pb.h"

#include "HandlerInterface.h"

using namespace grpc;
using namespace simpleStream;

class UploadHandler : public HandlerInterface
{
public:
    UploadHandler(HandlerTag tag,
        SimpleStream::Stub* stub,
        grpc::CompletionQueue* cq);

    ~UploadHandler() override = default;

    UploadHandler(const UploadHandler&) = delete;
    UploadHandler& operator=(const UploadHandler&) = delete;
    UploadHandler(UploadHandler&&) = delete;
    UploadHandler& operator=(UploadHandler&&) = delete;

    bool onNext(bool ok) override;

    void cancel() override;

protected:
    enum class CallState
    {
        NewCall,
        SendingHeader,
        SendingFile,
        ExpectingResponse,
        CallComplete
    };

    void handleNewCallState();
    void handleSendingHeaderState();
    void handleSendingFileState();
    void handleExpectingResponseState();
    void handleCallCompleteState();

    HandlerTag tag_;

    SimpleStream::Stub* stub_;
    grpc::CompletionQueue* cq_;
    grpc::ClientContext ctx_;

    std::unique_ptr<grpc::ClientAsyncWriter<SendRequest>> rpc_;

    SendRequest request_;
    SendResponse response_;
    grpc::Status status_;

    CallState state_;

    std::ifstream fileReader_;

    std::string filename_;

    size_t bytesToSend_;
    size_t fulSize_;
};


class DownloadHandler : public HandlerInterface
{
public:
    DownloadHandler(HandlerTag tag,
        SimpleStream::Stub* stub,
        grpc::CompletionQueue* cq,
        size_t readSize
    );

    ~DownloadHandler() override = default;

    DownloadHandler(const DownloadHandler&) = delete;
    DownloadHandler& operator=(const DownloadHandler&) = delete;
    DownloadHandler(DownloadHandler&&) = delete;
    DownloadHandler& operator=(DownloadHandler&&) = delete;

    bool onNext(bool ok) override;

    void cancel() override;

protected:
    enum class CallState
    {
        NewCall,
        SendingRequest,
        ExpectingHeader,
        ReceivingFile,
        CallComplete
    };

    void handleNewCallState();
    void handleSendingRequestState();
    void handleExpectingHeaderState();
    void handleReceivingFileState();
    void handleCallCompleteState();

    HandlerTag tag_;

    SimpleStream::Stub* stub_;
    grpc::CompletionQueue* cq_;
    grpc::ClientContext ctx_;

    std::unique_ptr<grpc::ClientAsyncReader<RecvResponse>> rpc_;

    RecvRequest request_;
    RecvResponse response_;
    grpc::Status status_;

    CallState state_;

    std::ofstream fileWriter_;

    std::string filename_;

    size_t bytesReceived_;
    size_t fulSize_ = 0;
};