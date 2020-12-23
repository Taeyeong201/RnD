#include <iostream>
#include <memory>
#include <string>


#define VERSION 2



#if VERSION == 1
#include "EchoClient.cpp"
#else
#include "EchoClient2.cpp"
#endif

int main() {
	EchoClient client(
		grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials())
	);
#if VERSION == 2
    // 무한 반복되는 리더 스레드 생성
    std::thread thread_ = std::thread(&EchoClient::AsyncCompleteRpc, &client);
#endif

    while (true) {
        std::string text;
        std::cout << "text : ";
        std::cin >> text;
#if VERSION == 1
        EchoMessage res = client.Echo(text);
        if (!res.echo().empty()) {
            std::cout << "recv : " << res.echo()
                << '(' << res.size() << ','
                << res.ByteSizeLong() << ')' << std::endl;
        }
        else {
            system("PAUSE");
            break;
        }
#else
        client.Echo(text);
#endif
    }

#if VERSION == 2
    thread_.join();
#endif

	return 0;
}