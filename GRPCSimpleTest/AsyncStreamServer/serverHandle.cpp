#include "serverHandle.h"


ServerUploadHandler::ServerUploadHandler(HandlerTag tag,
	HandlerManager* handlerManager,
	SimpleStream::AsyncService* service,
	grpc::ServerCompletionQueue* cq)
	: tag_(tag)
	, handlerManager_(handlerManager)
	, state_(CallState::NewCall)
	, errorStatus_(nullptr)
	, service_(service)
	, cq_(cq)
	, responder_(&ctx_)
	, severRecvFilePath_("ServerRecvStream.txt")
{
	this->onNext(true);
}

void ServerUploadHandler::cancel() {
	// todo
}

bool ServerUploadHandler::onNext(bool ok)
{
	try {
		if (state_ == CallState::CallComplete) {
			gpr_log(GPR_DEBUG, "[%p] The call has been completed", tag_);
			return false;
		}
		else if (ok) {
			if (state_ == CallState::NewCall) {
				this->handleNewCallState();
			}
			else if (state_ == CallState::ExpectingHeader) {
				this->handleExpectingHeaderState();
			}
			else if (state_ == CallState::ReceivingHeader) {
				this->handleReceivingHeaderState();
			}
			else if (state_ == CallState::ReceivingFile) {
				this->handleReceivingFileState();
			}
		}
		else {
			if (state_ == CallState::ExpectingHeader) {
				// server has been shut down before receiving a matching UploadRequest
				gpr_log(GPR_DEBUG, "[%p] Server has been shut down before receiving a matching UploadRequest", tag_);
				return false;
			}
			else {
				auto& rpcStatus = (state_ == CallState::ReceivingFile) ? grpc::Status::OK
					: grpc::Status::CANCELLED;
				state_ = CallState::CallComplete;
				responder_.Finish(response_, rpcStatus, tag_);
			}
		}
	}
	catch (std::exception& e) {
		errorStatus_ = &RequestStatus::UnknownError;
		gpr_log(GPR_ERROR, "[%p] Upload processing error: %s", tag_, e.what());
	}
	catch (...) {
		errorStatus_ = &RequestStatus::UnknownError;
		gpr_log(GPR_ERROR, "[%p] Upload processing error: unknown exception caught", tag_);
	}

	if (errorStatus_ != nullptr) {
		state_ = CallState::CallComplete;
		responder_.FinishWithError(*errorStatus_, tag_);
	}

	return true;
}

void ServerUploadHandler::handleNewCallState()
{
	state_ = CallState::ExpectingHeader;
	service_->RequestServerUpload(&ctx_, &responder_, cq_, cq_, tag_);
}


void ServerUploadHandler::handleExpectingHeaderState()
{
	try {
		handlerManager_->addHandler<ServerUploadHandler>(handlerManager_, service_, cq_);
	}
	catch (...) {
		gpr_log(GPR_ERROR, "[%p] Failed to create UploadRequest handler, no new UploadRequest's can be processed", tag_);
	}

	state_ = CallState::ReceivingHeader;
	responder_.Read(&request_, tag_);
}


void ServerUploadHandler::handleReceivingHeaderState()
{
	if (request_.has_header()) {
		std::cout << std::endl;
		std::cout << "type : " << request_.header().type() << std::endl;
		std::cout << "size : " << request_.header().size() << std::endl;

		fileWriter_.open(
			severRecvFilePath_.data(),
			std::ios::binary);

		response_.set_type(request_.header().type());
		fulSize_ = request_.header().size();
		response_.set_size(0);
		

		state_ = CallState::ReceivingFile;
		request_.Clear();
		responder_.Read(&request_, tag_);
	}
	else {
		errorStatus_ = &RequestStatus::FileHeaderExpected;
	}
}


void ServerUploadHandler::handleReceivingFileState()
{
	if (request_.has_payload()) {
		auto& chunkData = request_.payload().data();
		if (!chunkData.empty()) {
			if (fileWriter_.is_open()) {
				fileWriter_.write(
					chunkData.data(),
					chunkData.size()
				);
			}
			else {
				throw std::runtime_error("file open fail");
			}
			size_t readSize = response_.size() + chunkData.size();
			double percent = (double)readSize / (double)fulSize_;
			printf("\r download...%.2f%%", percent * 100.0);

			response_.set_size(readSize);
		}

		request_.Clear();
		responder_.Read(&request_, tag_);
	}
	else {
		errorStatus_ = &RequestStatus::FileChunkExpected;
	}
}

// ----------------------- Download Handle ----------------------

ClientDownloadHandler::ClientDownloadHandler(
	HandlerTag tag, 
	HandlerManager* handlerManager, 
	SimpleStream::AsyncService* service, 
	grpc::ServerCompletionQueue* cq)
	: tag_(tag)
	, handlerManager_(handlerManager)
	, state_(CallState::NewCall)
	, errorStatus_(nullptr)
	, service_(service)
	, cq_(cq)
	, responder_(&ctx_)
	, bytesToSend_(0)
{
	this->onNext(true);
}

bool ClientDownloadHandler::onNext(bool ok)
{
	try {
		if (state_ == CallState::CallComplete) {
			gpr_log(GPR_DEBUG, "[%p] The call has been completed", tag_);
			return false;
		}
		else if (ok) {
			if (state_ == CallState::NewCall) {
				this->handleNewCallState();
			}
			else if (state_ == CallState::ExpectingRequest) {
				this->handleExpectingRequestState();
			}
			else if (state_ == CallState::SendingFile) {
				this->handleSendingFileState();
			}
		}
		else {
			if (state_ == CallState::ExpectingRequest) {
				// server has been shut down before receiving a matching DownloadRequest
				gpr_log(GPR_DEBUG, "[%p] Server has been shut down before receiving a matching DownloadRequest", tag_);
				return false;
			}
			else {
				gpr_log(GPR_INFO, "[%p] DownloadRequest processing aborted: call is cancelled or connection is dropped", tag_);
				return false;
			}
		}
	}
	catch (std::exception& e) {
		errorStatus_ = &RequestStatus::UnknownError;
		gpr_log(GPR_ERROR, "[%p] Download processing error: %s", tag_, e.what());
	}
	catch (...) {
		errorStatus_ = &RequestStatus::UnknownError;
		gpr_log(GPR_ERROR, "[%p] Download processing error: unknown exception caught", tag_);
	}

	if (errorStatus_ != nullptr) {
		state_ = CallState::CallComplete;
		responder_.Finish(*errorStatus_, tag_);
	}

	return true;
}

void ClientDownloadHandler::cancel()
{
	//TODO
}

void ClientDownloadHandler::handleNewCallState()
{
	state_ = CallState::ExpectingRequest;
	service_->RequestClientDownload(&ctx_, &request_, &responder_, cq_, cq_, tag_);
}

void ClientDownloadHandler::handleExpectingRequestState()
{
	const unsigned long DefaultChunkSize = 4 * 1024; // 4K

	try {
		handlerManager_->addHandler<ClientDownloadHandler>(handlerManager_, service_, cq_);
	}
	catch (...) {
		gpr_log(GPR_ERROR, "[%p] Failed to create DownloadRequest handler, no new DownloadRequest's can be processed", tag_);
	}
	std::cout << std::endl;

	std::cout << "type : " << request_.type() << std::endl;
	std::cout << "size : " << request_.size() << std::endl;
	size_t requestSize = request_.size();

	gpr_log(GPR_INFO, "[%p] Received DownloadRequest: file size [%d]", tag_, request_.size());

	fileReader_.open("ServerSendStream.txt", std::ios::binary | std::ios::ate);

	size_t fileSize = 0;
	size_t readFileSize = 0;
	auto endpos = fileReader_.tellg();
	fileReader_.seekg(0);
	if (endpos > 0) {
		fileSize = static_cast<decltype(fileSize)>(endpos);
	}
	fulSize = fileSize;

	if ((requestSize == 0) || requestSize > fileSize) {
		std::cout << "resize : " << fileSize << std::endl;
		bytesToSend_ = fileSize;
	}
	else {
		bytesToSend_ = requestSize;
	}

	response_.mutable_header()->set_type(request_.type());
	response_.mutable_header()->set_size(bytesToSend_);

	state_ = CallState::SendingFile;
	responder_.Write(response_, tag_);
}

void ClientDownloadHandler::handleSendingFileState()
{
	bool finish = false;

	size_t chunkSize = 4096;
	if (chunkSize >= bytesToSend_) {
		chunkSize = bytesToSend_;
		finish = true;
	}

	response_.Clear();
	auto buffer = response_.mutable_payload()->mutable_data();
	buffer->resize(chunkSize);

	auto& buf = *buffer;

	if(!fileReader_.eof()) {
		fileReader_.read(&buf[0], buffer->size());
	}

	bytesToSend_ -= chunkSize;

	double percent = (double)(fulSize - bytesToSend_) / (double)fulSize;
	printf("\r upload...%.2f%%", percent * 100.0);

	if (finish) {
		state_ = CallState::CallComplete;
		responder_.WriteAndFinish(response_, grpc::WriteOptions(), grpc::Status::OK, tag_);
		fileReader_.close();
		std::cout << std::endl;
	}
	else {
		responder_.Write(response_, tag_);
	}
}
