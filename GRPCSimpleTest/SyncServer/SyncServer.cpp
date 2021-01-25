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

// ���� ���� �ڿ� �ִ� �� �� �������Դϴ�.
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
		// ���� ��Ŀ���� ���� ������ �ּҸ� �����մϴ�.
		builder.AddListeningPort(host, grpc::InsecureServerCredentials());
		// "����"�� ����� �ν��Ͻ��� ����մϴ�.
		// Ŭ���̾�Ʈ. �� ��� *�����* ���񽺿� �ش��մϴ�.
		builder.RegisterService(this);
		// ���������� ������ �����մϴ�.
		std::unique_ptr<Server> server(builder.BuildAndStart());
		std::cout << "Server listening on " << host << std::endl;

		// ������ ����� ������ ���. �Ϻ� �ٸ� ������� ������ ���ƾ� �մϴ�.
		// �� ȣ���� �ٽ� ���ƿ� ������ ������ �����ؾ� �մϴ�.
		server->Wait();
	}


};

int main() {
	std::string address("0.0.0.0:50051");

	std::shared_ptr<EchoImpl> echoServer = EchoImpl::getInstance();
	echoServer->run(address);

	return 0;
}