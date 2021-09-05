#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>

#include <grpc++\grpc++.h>

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

		std::ifstream fileReader(
			severSendFilePath.data(),
			std::ios::binary | std::ios::ate
		);
		if (fileReader.is_open()) {
			size_t fileSize = 0;
			auto endpos = fileReader.tellg();
			fileReader.seekg(0);
			if (endpos > 0) {
				fileSize = static_cast<decltype(fileSize)>(endpos);
			}
			if ((size == 0) || size > fileSize) {
				std::cout << "resize : " << fileSize << std::endl;
			}
			else {
				fileSize = size;
			}

			remainSize_ = fileSize;

			RecvResponse rep;
			rep.mutable_header()->set_type(request->type());
			rep.mutable_header()->set_size(fileSize);
			writer->Write(rep);
			rep.Clear();

			auto buffer = rep.mutable_payload()->mutable_data();
			bufferSize_ = 4096;
			buffer->resize(bufferSize_);
			auto& buf = *buffer;

			double percent = 0;

			while (!fileReader.eof()) {
				bool finish = false;

				if (bufferSize_ >= remainSize_) {
					bufferSize_ = remainSize_;
					buffer->resize(bufferSize_);
					finish = true;
				}

				fileReader.read(&buf[0], bufferSize_);
				remainSize_ -= bufferSize_;

				writer->Write(rep);

				percent = (double)(fileSize - remainSize_) / (double)fileSize;
				printf("\r upload...%.2f%%", percent * 100.0);

				if (finish) break;
			}
			std::cout << std::endl;

			fileReader.close();
		}
		else {
			StreamPayload payload;
			payload.set_data("null");
			RecvResponse rep;
			
			writer->Write(rep);
		}
		return Status::OK;
	}

	void read(const std::string& filename, std::string& data)
	{
		std::ifstream file(filename.c_str(), std::ios::in);

		if (file.is_open())
		{
			std::stringstream ss;
			ss << file.rdbuf();

			file.close();

			data = ss.str();
		}

		return;
	}
private:
	int bufferSize_ = 0;

	size_t remainSize_ = 0;

	std::string severSendFilePath;
	std::string severRecvFilePath;
};

int main() {
	std::string server_address("0.0.0.0:50051");
	SimpleStreamImpl service;

	std::string key;
	std::string cert;
	std::string root;

	service.read("server.key", key);
	service.read("server.crt", cert);
	//service.read("rootCA.pem", root);

	ServerBuilder builder;
	grpc::SslServerCredentialsOptions::PemKeyCertPair keycert = { key, cert };
	grpc::SslServerCredentialsOptions sslOptions(GRPC_SSL_DONT_REQUEST_CLIENT_CERTIFICATE);

	//sslOptions.pem_root_certs = root;
	sslOptions.pem_key_cert_pairs.push_back(keycert);

	builder.AddListeningPort(server_address, grpc::SslServerCredentials(sslOptions));
	builder.RegisterService(&service);
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();

	return 0;
}