#include <iostream>
#include <string>
#include <memory>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <thread>

#include <forward_list>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include <signal.h>


#include "stream.grpc.pb.h"

#include "serverHandle.h"

using namespace grpc;
using namespace simpleStream;

std::atomic_bool shutdownRequested(false);

class AsyncStreamServer final {
public:
	AsyncStreamServer(unsigned long threadNumber)
		: shutdownFlag_(false)
	{
		if (threadNumber == 0) {
			throw std::logic_error("Worker threads number has to be greater than 0");
		}

		workerThreads_.resize(threadNumber);
	}
	~AsyncStreamServer() {
		this->stop();
	}

	AsyncStreamServer(const AsyncStreamServer&) = delete;
	AsyncStreamServer& operator=(const AsyncStreamServer&) = delete;
	AsyncStreamServer(AsyncStreamServer&&) = delete;
	AsyncStreamServer& operator=(AsyncStreamServer&&) = delete;

	void start() {
		std::string address("0.0.0.0:50051");

		ServerBuilder builder;
		builder.AddListeningPort(address, InsecureServerCredentials());
		builder.RegisterService(&service_);
		cq_ = builder.AddCompletionQueue();
		server_ = builder.BuildAndStart();

		std::cout << "Server listening on " << address << std::endl;

		handlerManager_.addHandler<ServerUploadHandler>(&handlerManager_, &service_, cq_.get());
		handlerManager_.addHandler<ClientDownloadHandler>(&handlerManager_, &service_, cq_.get());

		for (auto& worker : workerThreads_) {
			std::thread t(&AsyncStreamServer::processMessages, this);
			worker.swap(t);
		}

	}

	void stop() {
		shutdownFlag_.store(true, std::memory_order_release);

		gpr_log(GPR_DEBUG, "Shutting down server");
		server_->Shutdown();

		gpr_log(GPR_DEBUG, "Shutting down notification queue");
		cq_->Shutdown();

		gpr_log(GPR_DEBUG, "Waiting for worker threads to shut down");
		for (auto& worker : workerThreads_) {
			worker.join();
		}

		// drain the queue
		gpr_log(GPR_DEBUG, "Draining notification queue");
		void* ignoredTag = nullptr;
		bool ok = false;
		while (cq_->Next(&ignoredTag, &ok));
	}

	void processMessages() {
		try {
			void* tag = nullptr;
			bool ok = false;
			while (!shutdownFlag_.load(std::memory_order_acquire)) {
				if (cq_->Next(&tag, &ok)) {
					if (tag) {
						auto& handler = handlerManager_.getHandler(static_cast<HandlerTag>(tag));
						if (handler) {
							auto res = handler->onNext(ok);
							if (!res) {
								handler.reset();
							}
						}
					}
					else {
						gpr_log(GPR_ERROR, "Invalid tag delivered by notification queue");
					}
				}
				else {
					// notification queue has been shut down
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

		gpr_log(GPR_DEBUG, "Worker thread is shutting down");
	}

private:
	std::unique_ptr<ServerCompletionQueue> cq_;
	SimpleStream::AsyncService service_;
	std::unique_ptr<Server> server_;
	std::vector<std::thread> workerThreads_;
	std::atomic_bool shutdownFlag_;
	HandlerManager handlerManager_;
};

using namespace std::chrono_literals;

void signalHandler(int signo)
{
	shutdownRequested.store(true, std::memory_order_release);
}

void setSignalHandler()
{
	typedef void (*SignalHandlerPointer)(int);

	SignalHandlerPointer previousHandler;
	previousHandler = signal(SIGINT, signalHandler);
}

int main() {

	setSignalHandler();

	AsyncStreamServer server(1);
	server.start();

	while (!shutdownRequested.load(std::memory_order_acquire)) {
		std::this_thread::sleep_for(1s);
	}

	return 0;
}