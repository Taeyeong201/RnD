#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpc/support/log.h>

#include "echo.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using namespace simple;

// 서버 동작 뒤에 있는 논리 및 데이터입니다.
class EchoImpl final : public EchoService::Service {

public:
	explicit EchoImpl() {}

	static std::shared_ptr<EchoImpl> getInstance() {
		return std::make_shared<EchoImpl>();
	}

	Status Echo(
		ServerContext* context,
		const Message* request,
		EchoMessage* response
	) override {
		std::string prefix("Echo ");
		std::cout << "size : " << request->size() << std::endl;
		prefix.append(request->text());

		response->set_echo(prefix);
		response->set_size(prefix.length());

		return Status::OK;
	}

	void run(const std::string& host) {
		grpc::EnableDefaultHealthCheckService(true);
		grpc::reflection::InitProtoReflectionServerBuilderPlugin();
		ServerBuilder builder;
		// 인증 메커니즘 없이 지정된 주소를 수신합니다.
		builder.AddListeningPort(host, grpc::InsecureServerCredentials());
		// "서비스"를 통신할 인스턴스로 등록합니다.
		// 클라이언트. 이 경우 *동기식* 서비스에 해당합니다.
		builder.RegisterService(this);
		// 마지막으로 서버를 조립합니다.
		std::unique_ptr<Server> server(builder.BuildAndStart());
		std::cout << "Server listening on " << host << std::endl;

		// 서버가 종료될 때까지 대기. 일부 다른 스레드는 다음과 같아야 합니다.
		// 이 호출이 다시 돌아올 때까지 서버를 종료해야 합니다.
		server->Wait();
	}


};

int main() {
	std::string address("0.0.0.0:50051");

	std::shared_ptr<EchoImpl> echoServer = EchoImpl::getInstance();
	echoServer->run(address);

	return 0;
}