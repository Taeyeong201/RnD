#include <iostream>
#include <memory>
#include <string>
#include <fstream>

#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include "stream.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using namespace simpleStream;

class SimpleStreamImpl final : public SimpleStream::Service {
public:
	explicit SimpleStreamImpl() :
		severSendFilePath("ServerSendStream.txt"),
		severRecvFilePath("ServerRecvStream.txt")
	{}

	Status ServerUpload(
		ServerContext* context,
		ServerReader<SendRequest>* reader,
		SendResponse* response) override
	{
		SendRequest req;
		size_t totalSize = 0;
		size_t readSize = 0;
		std::string type;
		std::ofstream recvStream(
			severRecvFilePath.data(),
			std::ios::binary
		);
		while (reader->Read(&req)) {
			if (req.has_header()) {
				std::cout << "type : " << req.header().type() << std::endl;
				std::cout << "size : " << req.header().size() << std::endl;
				totalSize = req.header().size();
				type = req.header().type();
			}
			else {
				if (recvStream.is_open()) {
					recvStream.write(
						req.payload().data().data(),
						req.payload().data().size()
					);
					readSize += req.payload().data().size();
					double percent = (double)readSize / (double)totalSize;
					printf("\r download...%.2f%%", percent * 100.0);
				}
				else {
					type = std::string("err");
					std::cerr << "file is not open" << std::endl;
					break;
				}
			}
		}
		std::cout << std::endl;

		if (type.empty()) response->set_type("payload");
		else response->set_type(type);
		response->set_size(readSize);
		recvStream.close();

		return Status::OK;
	}

	Status ClientDownload(
		ServerContext* context,
		const RecvRequest* request,
		ServerWriter<RecvResponse>* writer) override
	{
		int size = request->size();
		std::cout << "type : " << request->type() << std::endl;
		std::cout << "size : " << request->size() << std::endl;

		std::ifstream sendStream(
			severSendFilePath.data(),
			std::ios::binary | std::ios::ate
		);
		if (sendStream.is_open()) {
			size_t fileSize = 0;
			size_t readFileSize = 0;
			auto endpos = sendStream.tellg();
			sendStream.seekg(0);
			if (endpos > 0) {
				fileSize = static_cast<decltype(fileSize)>(endpos);
			}
			if ((size == 0) || size > fileSize) {
				std::cout << "resize : " << fileSize << std::endl;
			}
			else {
				fileSize = size;
			}
			
			RecvResponse rep;
			rep.mutable_header()->set_type(request->type());
			rep.mutable_header()->set_size(fileSize);
			writer->Write(rep);
			rep.Clear();

			auto buffer = rep.mutable_payload()->mutable_data();
			buffer->resize(bufferSize_);
			size_t last = fileSize % bufferSize_;
			auto& buf = *buffer;

			while (!sendStream.eof()) {
				if ((readFileSize + buffer->size()) < fileSize) {
					sendStream.read(&buf[0], buffer->size());
					readFileSize += buffer->size();
					writer->Write(rep);
				}
				else if (readFileSize == fileSize) {
					sendStream.read(&buf[0], 1);
					break;
				}
				else {
					buffer->resize(last);
					sendStream.read(&buf[0], last);
					readFileSize += last;
					writer->Write(rep);
				}
				double percent = (double)readFileSize / (double)fileSize;
				printf("\r upload...%.2f%%", percent * 100.0);
			}
			std::cout << std::endl;

			sendStream.close();
		}
		else {
			StreamPayload payload;
			payload.set_data("null");
			RecvResponse rep;
			
			writer->Write(rep);
		}
		return Status::OK;
	}

private:
	int bufferSize_ = 4096;
	std::string severSendFilePath;
	std::string severRecvFilePath;
};

int main() {
	std::string server_address("0.0.0.0:50051");
	SimpleStreamImpl service;

	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();

	return 0;
}