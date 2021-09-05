#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <exception>
#include <fstream>

#include <grpc/support/log.h>

#include "HandlerManager.hpp"
#include "RequestStatus.h"

#include "stream.grpc.pb.h"

using namespace grpc;
using namespace simpleStream;

class ServerUploadHandler : public HandlerInterface {
public:
	ServerUploadHandler(HandlerTag tag,
		HandlerManager* handlerManager,
		SimpleStream::AsyncService* service,
		grpc::ServerCompletionQueue* cq);

	ServerUploadHandler(const ServerUploadHandler&) = delete;
	ServerUploadHandler& operator=(const ServerUploadHandler&) = delete;
	ServerUploadHandler(ServerUploadHandler&&) = delete;
	ServerUploadHandler& operator=(ServerUploadHandler&&) = delete;

	bool onNext(bool ok) override;

	void cancel() override;

protected:
	enum class CallState
	{
		NewCall,
		ExpectingHeader,
		ReceivingHeader,
		ReceivingFile,
		CallComplete
	};

	void handleNewCallState();
	void handleExpectingHeaderState();
	void handleReceivingHeaderState();
	void handleReceivingFileState();

	std::string severRecvFilePath_;
	std::ofstream fileWriter_;

	HandlerTag tag_;
	HandlerManager* handlerManager_;

	CallState state_;
	const grpc::Status* errorStatus_;

	SimpleStream::AsyncService* service_;
	grpc::ServerCompletionQueue* cq_;
	grpc::ServerContext             ctx_;

	SendRequest                   request_;
	SendResponse                  response_;

	grpc::ServerAsyncReader<SendResponse, SendRequest> responder_;

	size_t fulSize_;
};


class ClientDownloadHandler : public HandlerInterface
{
public:
    ClientDownloadHandler(HandlerTag tag,
        HandlerManager* handlerManager,
        SimpleStream::AsyncService* service,
        grpc::ServerCompletionQueue* cq);

    ClientDownloadHandler(const ClientDownloadHandler&) = delete;
    ClientDownloadHandler& operator=(const ClientDownloadHandler&) = delete;
    ClientDownloadHandler(ClientDownloadHandler&&) = delete;
    ClientDownloadHandler& operator=(ClientDownloadHandler&&) = delete;

    bool onNext(bool ok) override;

    void cancel() override;

protected:
    enum class CallState
    {
        NewCall,
        ExpectingRequest,
        SendingFile,
        CallComplete
    };

    void handleNewCallState();
    void handleExpectingRequestState();
    void handleSendingFileState();

    HandlerTag tag_;
    HandlerManager* handlerManager_;

    std::ifstream fileReader_;

    CallState state_;
    const grpc::Status* errorStatus_;

    SimpleStream::AsyncService* service_;
    grpc::ServerCompletionQueue* cq_;
    grpc::ServerContext             ctx_;

	RecvRequest request_;
	RecvResponse response_;

    grpc::ServerAsyncWriter<RecvResponse> responder_;

    size_t bytesToSend_;
    size_t fulSize;
};