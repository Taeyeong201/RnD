#include <iostream>
#include <memory>
#include <string>
#include <fstream>

#include <grpcpp/grpcpp.h>

#include "AudioStream.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using namespace LiveStream;

class testserver final :public AudioStream::Service {
public:
	explicit testserver() {}

	Status AudioStreamReqeust(
		ServerContext* context, 
		const AudioFormat* request, 
		ServerWriter<DataStream>* writer) override
	{
		std::cout << request->codec() << std::endl;
		std::cout << request->bitrate() << std::endl;
		std::cout << request->samplerate() << std::endl;

		DataStream rep;
		rep.mutable_header()->set_code(1);
		rep.mutable_header()->set_status("good");
		writer->Write(rep);
		rep.Clear();

		auto buffer = rep.mutable_payload()->mutable_data();

		while (true) {
			buffer->clear();
			buffer->append("asd");
			writer->Write(rep);
			Sleep(500);
		}
	}
};

int main() {
	std::string server_address("0.0.0.0:50051");
	testserver service;

	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();

	return 0;
}