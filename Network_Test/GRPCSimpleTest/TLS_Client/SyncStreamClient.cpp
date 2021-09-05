#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>

#include <grpc++\grpc++.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

#include "stream.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientWriter;
using grpc::Status;

using namespace simpleStream;

class SimpleStreamClient {
public:
	SimpleStreamClient(std::shared_ptr<Channel> channel)
		:
		stub_(SimpleStream::NewStub(channel)),
		sendFilePath("ClientSendStream.txt"),
		recvFilePath("ClientRecvStream.txt")
	{}

	void SendStreamData() {
		SendResponse reply;
		ClientContext context;
		std::ifstream fileReader(
			sendFilePath.data(),
			std::ios::binary | std::ios::ate
		);

		if (fileReader.is_open()) {
			std::unique_ptr<ClientWriter<SendRequest>> writer(
				stub_->ServerUpload(&context, &reply)
			);
			size_t readFileSize = 0;
			size_t fileSize = 0;
			auto endpos = fileReader.tellg();
			fileReader.seekg(0);
			if (endpos > 0) {
				fileSize = static_cast<decltype(fileSize)>(endpos);
			}

			remainSize_ = fileSize;
			fulSize = fileSize;

			SendRequest req;
			req.mutable_header()->set_type("textFile");
			req.mutable_header()->set_size(fileSize);
			writer->Write(req);
			req.Clear();



			auto buffer = req.mutable_payload()->mutable_data();
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

				writer->Write(req);

				percent = (double)(fulSize - remainSize_) / (double)fulSize;
				printf("\r upload...%.2f%%", percent * 100.0);

				if (finish) break;
			}
			std::cout << std::endl;
			fileReader.close();

			writer->WritesDone();
			Status status = writer->Finish();
			if (status.ok())
				std::cout << "finish file upload(" << reply.size() << ")" << std::endl;
			else 
				std::cout << "upload failed" << std::endl;
		}
		else {
			std::cout << "file is not open" << std::endl;
		}

		

	}

	void RecvStreamData(const size_t& readSize) {
		RecvRequest req;
		RecvResponse response;
		ClientContext context;
		std::ofstream recvStream(
			recvFilePath.data(),
			std::ios::binary
		);
		req.set_type("textFile");
		req.set_size(readSize);
		if (recvStream.is_open()) {
			size_t totalSize = 0;
			size_t readSize = 0;
			std::unique_ptr<ClientReader<RecvResponse>> reader(
				stub_->ClientDownload(&context, req));
			while (reader->Read(&response)) {
				if (response.has_header()) {
					std::cout << "type : " << response.header().type() << std::endl;
					std::cout << "size : " << response.header().size() << std::endl;
					totalSize = response.header().size();
				}
				else {
					recvStream.write(
						response.payload().data().data(),
						response.payload().data().size()
					);
					readSize += response.payload().data().size();

					double percent = (double)readSize / (double)totalSize;
					printf("\r download...%.2f%%", percent * 100.0);
				}
			}
			std::cout << std::endl;
		}
		else {
			std::cout << "file is not open" << std::endl;
		}
	}


private:
	size_t bufferSize_ = 4096;
	std::string sendFilePath;
	std::string recvFilePath;
	std::unique_ptr<SimpleStream::Stub> stub_;

	size_t fulSize = 0;
	size_t remainSize_ = 0;
};

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

int main() {
	std::string cert;
	std::string key;
	std::string root;
	std::string server{ "localhost:50051" };

	//read("server.pem", cert);
	//read("client.key", key);
	read("rootCA.crt", root);

	grpc::SslCredentialsOptions opts =
	{
		//root,
		root
	};


	SimpleStreamClient client(
		grpc::CreateChannel(
			"localhost:50051",
			grpc::SslCredentials(opts)
		)
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
			client.SendStreamData();
		}
		else if (anser.compare("2") == 0) {
			std::string anser;
			std::cout << "read size (all : 0) >>>> ";
			std::cin >> anser;
			int size = stoi(anser);
			client.RecvStreamData(size);
		}
		else {
			break;
		}
	}

	return 0;
}