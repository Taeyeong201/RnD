#include "clientHandle.h"

UploadHandler::UploadHandler(
	HandlerTag tag, 
	SimpleStream::Stub* stub, 
	grpc::CompletionQueue* cq)
	: tag_(tag)
	, stub_(stub)
	, cq_(cq)
	, state_(CallState::NewCall)
	, filename_("ClientSendStream.txt")
	, bytesToSend_(0)
{
	this->onNext(true);
}

bool UploadHandler::onNext(bool ok)
{
    try {
        if (state_ == CallState::CallComplete) {
            this->handleCallCompleteState();
            return false; //TODO comment
        }
        else if (ok) {
            if (state_ == CallState::NewCall) {
                this->handleNewCallState();
            }
            else if (state_ == CallState::SendingHeader) {
                this->handleSendingHeaderState();
            }
            else if (state_ == CallState::SendingFile) {
                this->handleSendingFileState();
            }
            else if (state_ == CallState::ExpectingResponse) {
                this->handleExpectingResponseState();
            }
        }
        else {
            state_ = CallState::CallComplete;
            rpc_->Finish(&status_, tag_);
        }

        return true;
    }
    catch (std::exception& e) {
        gpr_log(GPR_ERROR, "Upload processing error: %s", e.what());
    }
    catch (...) {
        gpr_log(GPR_ERROR, "Upload processing error: unknown exception caught");
    }

    if (state_ == CallState::NewCall) {
        //TODO comment
        return false;
    }

    ctx_.TryCancel();

    return true;
}

void UploadHandler::cancel()
{
    ctx_.TryCancel();
}

void UploadHandler::handleNewCallState()
{
    fileReader_.open(filename_.data(), std::ios::binary | std::ios::ate);

    rpc_ = stub_->PrepareAsyncServerUpload(&ctx_, &response_, cq_);

    state_ = CallState::SendingHeader;
    rpc_->StartCall(tag_);
}

void UploadHandler::handleSendingHeaderState()
{
    size_t fileSize = 0;
    auto endpos = fileReader_.tellg();
    fileReader_.seekg(0);
    if (endpos > 0) {
        fileSize = static_cast<decltype(fileSize)>(endpos);
    }
    bytesToSend_ = fileSize;
    if (bytesToSend_ > 0) {
        state_ = CallState::SendingFile;
    }
    else {
        state_ = CallState::ExpectingResponse;
    }

    fulSize_ = fileSize;

    request_.mutable_header()->set_type("textFile");
    request_.mutable_header()->set_size(fileSize);

    rpc_->Write(request_, tag_);
}

void UploadHandler::handleSendingFileState()
{
    const unsigned long long DefaultChunkSize = 4 * 1024; // 4K

    auto chunkSize = std::min(DefaultChunkSize, bytesToSend_);

    request_.Clear();
    auto buffer = request_.mutable_payload()->mutable_data();
    buffer->resize(chunkSize);
    auto& buf = *buffer;

    if (!fileReader_.eof()) {
        fileReader_.read(&buf[0], buffer->size());
    }
    bytesToSend_ -= chunkSize;

    grpc::WriteOptions writeOptions;
    if (bytesToSend_ > 0) {
        state_ = CallState::SendingFile;
        double percent = (double)(fulSize_ - bytesToSend_) / (double)fulSize_;
        printf("\r upload...%.2f%%", percent * 100.0);
    }
    else {
        state_ = CallState::ExpectingResponse;
        writeOptions.set_last_message();
    }

    rpc_->Write(request_, writeOptions, tag_);
}

void UploadHandler::handleExpectingResponseState()
{
    state_ = CallState::CallComplete;
    rpc_->Finish(&status_, tag_);
}

void UploadHandler::handleCallCompleteState()
{
    double percent = (double)(fulSize_ - bytesToSend_) / (double)fulSize_;
    printf("\r upload...%.2f%%\n", percent * 100.0);
    switch (status_.error_code()) {
    case grpc::OK:
    {
        auto bytesSent = fulSize_;
        std::cout << "[" << filename_ << "]: upload complete: " << bytesSent << " bytes send" << std::endl;
    }
    break;

    case grpc::CANCELLED:
        std::cout << "[" << filename_ << "]: upload cancelled" << std::endl;
        break;

    default:
        std::cout << "[" << filename_ << "]: upload failed: " << status_.error_message() << std::endl;
        break;
    }
}


// --------------------------- Download --------------------------------

DownloadHandler::DownloadHandler(
    HandlerTag tag, 
    SimpleStream::Stub* stub, 
    grpc::CompletionQueue* cq,
    size_t readSize)
    : tag_(tag)
    , stub_(stub)
    , cq_(cq)
    , state_(CallState::NewCall)
    , filename_("ClientRecvStream.txt")
    , bytesReceived_(0)
    , fulSize_(readSize)
{
    this->onNext(true);
}

bool DownloadHandler::onNext(bool ok)
{
    try {
        if (ok) {
            if (state_ == CallState::NewCall) {
                this->handleNewCallState();
            }
            else if (state_ == CallState::SendingRequest) {
                this->handleSendingRequestState();
            }
            else if (state_ == CallState::ExpectingHeader) {
                this->handleExpectingHeaderState();
            }
            else if (state_ == CallState::ReceivingFile) {
                this->handleReceivingFileState();
            }
            else if (state_ == CallState::CallComplete) {
                this->handleCallCompleteState();
                return false; //TODO comment
            }
        }
        else {
            state_ = CallState::CallComplete;
            rpc_->Finish(&status_, tag_);
        }

        return true;
    }
    catch (std::exception& e) {
        gpr_log(GPR_ERROR, "Download processing error: %s", e.what());
    }
    catch (...) {
        gpr_log(GPR_ERROR, "Download processing error: unknown exception caught");
    }

    if (state_ == CallState::NewCall) {
        //TODO comment
        return false;
    }

    ctx_.TryCancel();

    return true;
}

void DownloadHandler::cancel()
{
    ctx_.TryCancel();
}

void DownloadHandler::handleNewCallState()
{
    const std::size_t ServerDefaultChunkSize = 0;

    request_.set_type("textFile");
    request_.set_size(fulSize_);

    rpc_ = stub_->PrepareAsyncClientDownload(&ctx_, request_, cq_);

    state_ = CallState::SendingRequest;
    rpc_->StartCall(tag_);
}

void DownloadHandler::handleSendingRequestState()
{
    state_ = CallState::ExpectingHeader;
    rpc_->Read(&response_, tag_);
}

void DownloadHandler::handleExpectingHeaderState()
{
    if (response_.has_header()) {
        //TODO check filename?
        fileWriter_.open(
            filename_.data(),
            std::ios::binary);

        if (response_.header().size() > 0) {
            std::cout << "type : " << response_.header().type() << std::endl;
            std::cout << "size : " << response_.header().size() << std::endl;

            fulSize_ = response_.header().size();

            state_ = CallState::ReceivingFile;
            response_.Clear();
            rpc_->Read(&response_, tag_);
        }
        else {
            state_ = CallState::CallComplete;
            rpc_->Finish(&status_, tag_);
        }
    }
    else {
        throw std::runtime_error("File header expected");
    }
}

void DownloadHandler::handleReceivingFileState()
{
    if (response_.has_payload()) {
        auto& chunkData = response_.payload().data();
        if (chunkData.empty()) {
            gpr_log(GPR_INFO, "Received an empty FileChunk, ignoring");
        }
        else {
            fileWriter_.write(
                chunkData.data(),
                chunkData.size()
            );

            bytesReceived_ += chunkData.size();
            double percent = (double)bytesReceived_ / (double)fulSize_;
            printf("\r download...%.2f%%", percent * 100.0);
        }

        state_ = CallState::ReceivingFile;
        response_.Clear();
        rpc_->Read(&response_, tag_);
    }
    else {
        throw std::runtime_error("File chunk expected");
    }
}

void DownloadHandler::handleCallCompleteState()
{
    double percent = (double)bytesReceived_ / (double)fulSize_;
    printf("\r download...%.2f%%\n", percent * 100.0);

    switch (status_.error_code()) {
    case grpc::OK:
        std::cout << "[" << filename_ << "]: download complete: " << bytesReceived_ << " bytes received" << std::endl;
        break;

    case grpc::CANCELLED:
        std::cout << "[" << filename_ << "]: download cancelled" << std::endl;
        break;

    default:
        std::cout << "[" << filename_ << "]: download failed: " << status_.error_message() << std::endl;
        break;
    }
}
