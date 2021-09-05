
#include <iostream>
#include <memory>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include "echo.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using namespace simple;

class EchoClient {
public:
	EchoClient(std::shared_ptr<Channel> channel)
		: stub_(EchoService::NewStub(channel)) {}

    // Ŭ���̾�Ʈ�� ���̷ε带 �����ϰ� ������ �� �������� ������ �ٽ� �����մϴ�.
    EchoMessage echo(const std::string& msg) {
        // ������ ������ ������.
        Message request;
        request.set_size(msg.length());
        request.set_text(msg);

        // �������� ����Ǵ� �����͸� ������ �����̳��Դϴ�.
        EchoMessage reply;

        /* 
        Ŭ���̾�Ʈ�� ���ؽ�Ʈ�Դϴ�.
        �߰� ������ ������ �����ϰų� 
        Ư�� RPC ������ �����ϴ� �� ����� �� �ֽ��ϴ�. 
        */
        ClientContext context;

        // ���� RPC�Դϴ�.
        Status status = stub_->Echo(&context, request, &reply);

        // ���¿� ���� ��ġ�մϴ�.
        if (status.ok()) {
            return reply;
        }
        else {
            std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
            return reply;
        }
	}

private:
	std::unique_ptr<EchoService::Stub> stub_;
};

int main() {
    /*
    Ŭ���̾�Ʈ�� �ν��Ͻ�ȭ�մϴ�. 
    ä���� �ʿ��ϸ� �� �� ���� RPC�� �����˴ϴ�. 
    ù��° �μ��� ���� ������ ������ ���� ������ �𵨸��մϴ�. 
    ä���� �������� �ʾ����� ��Ÿ���ϴ�(InsecureChannelCredentials() ����ϸ�)
    */
    EchoClient client(grpc::CreateChannel(
        "127.0.0.1:50051", grpc::InsecureChannelCredentials()));
    EchoClient client2(grpc::CreateChannel(
        "127.0.0.1:50051", grpc::InsecureChannelCredentials()));

    while (true) {
        std::string text;
        std::cout << "text : ";
        std::cin >> text;
        EchoMessage res = client.echo(text);
        EchoMessage res2 = client2.echo(text);
        if (!res2.echo().empty()) {
            std::cout << "recv2 : " << res2.echo()
                << '(' << res2.size() << ','
                << res2.ByteSizeLong() << ')' << std::endl;
        }
        else {
            system("PAUSE");
            break;
        }
        if (!res.echo().empty()) {
            std::cout << "recv : " << res.echo() 
                << '(' << res.size() << ',' 
                << res.ByteSizeLong() << ')' << std::endl;
        }
        else {
            system("PAUSE");
            break;
        }
    }


	return 0;
}