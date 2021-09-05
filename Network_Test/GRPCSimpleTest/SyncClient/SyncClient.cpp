
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

    // 클라이언트의 페이로드를 조립하고 전송한 후 서버에서 응답을 다시 제공합니다.
    EchoMessage echo(const std::string& msg) {
        // 서버로 보내는 데이터.
        Message request;
        request.set_size(msg.length());
        request.set_text(msg);

        // 서버에서 예상되는 데이터를 저장할 컨테이너입니다.
        EchoMessage reply;

        /* 
        클라이언트의 컨텍스트입니다.
        추가 정보를 서버에 전달하거나 
        특정 RPC 동작을 수정하는 데 사용할 수 있습니다. 
        */
        ClientContext context;

        // 실제 RPC입니다.
        Status status = stub_->Echo(&context, request, &reply);

        // 상태에 따라 조치합니다.
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
    클라이언트를 인스턴스화합니다. 
    채널이 필요하며 이 중 실제 RPC가 생성됩니다. 
    첫번째 인수에 의해 지정된 끝점에 대한 연결을 모델링합니다. 
    채널이 인증되지 않았음을 나타냅니다(InsecureChannelCredentials() 사용하면)
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