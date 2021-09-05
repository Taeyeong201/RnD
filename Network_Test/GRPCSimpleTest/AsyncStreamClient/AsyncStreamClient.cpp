#include <iostream>
#include <string>
#include <memory>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include "stream.grpc.pb.h"

#include "clientHandle.h"

using namespace grpc;
using namespace simpleStream;

class AsyncStreamClient final {
public:
	AsyncStreamClient(const std::shared_ptr<grpc::Channel>& channel)
		: stub_(SimpleStream::NewStub(channel)) {}
	~AsyncStreamClient() {
		this->cancel();

		cq_.Shutdown();

		// drain the queue
		void* ignoredTag = nullptr;
		bool ok = false;
		while (cq_.Next(&ignoredTag, &ok));
	}

	AsyncStreamClient(const AsyncStreamClient&) = delete;
	AsyncStreamClient& operator=(const AsyncStreamClient&) = delete;
	AsyncStreamClient(AsyncStreamClient&&) = delete;
	AsyncStreamClient& operator=(AsyncStreamClient&&) = delete;

	void upload() {
        handler_ = std::make_unique<UploadHandler>(&handler_, stub_.get(), &cq_);
        this->processMessages();

	}
	void download(size_t& readSize) {
        handler_ = std::make_unique<DownloadHandler>(&handler_, stub_.get(), &cq_, readSize);
        this->processMessages();

	}

	void cancel() {
        if (handler_) {
            handler_->cancel();
        }
	}

private:
	void processMessages() {
        try {
            void* tag = nullptr;
            bool ok = false;
            while (true) {
                if (cq_.Next(&tag, &ok)) {
                    if (tag) {
                        //TODO assert
                        auto res = handler_->onNext(ok);
                        if (!res) {
                            //TODO comment
                            handler_.reset();
                            break;
                        }
                    }
                    else {
                        gpr_log(GPR_ERROR, "Invalid tag delivered by notification queue");
                    }
                }
                else {
                    gpr_log(GPR_ERROR, "Notification queue has been shut down unexpectedly");
                    break;
                }
            }
        }
        catch (std::exception& e) {
            gpr_log(GPR_ERROR, "Caught exception: %s", e.what());
        }
        catch (...) {
            gpr_log(GPR_ERROR, "Caught unknown exception");
        }
	}

	std::unique_ptr<SimpleStream::Stub> stub_;
	grpc::CompletionQueue cq_;

	std::unique_ptr<HandlerInterface> handler_;
};

int main() {
    AsyncStreamClient client(
        grpc::CreateChannel(
            "127.0.0.1:50051", 
            grpc::InsecureChannelCredentials())
    );

	while (true) {
		std::cout << "----------------------" << std::endl;
		std::cout << "1. upload" << std::endl;
		std::cout << "2. download" << std::endl;
		std::cout << "3. exit" << std::endl;
		std::cout << ">>>> ";
		std::string anser;
		std::cin >> anser;
		if (anser.compare("1") == 0) {
			client.upload();
		}
		else if (anser.compare("2") == 0) {
			std::string anser;
			std::cout << "read size (all : 0) >>>> ";
			std::cin >> anser;
			size_t size = stoi(anser);
			client.download(size);
		}
		else {
			break;
		}
	}

	return 0;
}

