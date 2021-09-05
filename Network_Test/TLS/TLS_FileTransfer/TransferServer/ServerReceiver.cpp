#define BUF_SIZE 4096


#if 0
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <WinSock2.h>
#include <windows.h>
#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "Ws2_32.lib")
#define sleep(sec)  Sleep((sec)*1000)
#endif

typedef unsigned long long u64;
u64 GetMicroCounter();

// 파라메터 :  <port> <filename>
int main(int argc, char **argv) {
	u64 start, end;
	WSADATA wsaData;
	struct sockaddr_in local_addr;
	SOCKET  s_listen;

	if (argc != 3) {
		printf("Command parameter does not right. \n<port> <filename>\n");
		exit(1);
	}

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	if ((s_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket Creat Error.\n");
		exit(1);
	}

	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	local_addr.sin_port = htons(atoi(argv[1]));

	if (bind(s_listen, (SOCKADDR *)&local_addr, sizeof(local_addr)) == SOCKET_ERROR) {
		printf("Socket Bind Error.\n");
		exit(1);
	}

	if (listen(s_listen, 5) == SOCKET_ERROR) {
		printf("Socket Listen Error.\n");
		exit(1);
	}


	printf("This server is listening... \n");

	struct sockaddr_in client_addr;
	int len_addr = sizeof(client_addr);
	SOCKET  s_accept;
	int totalBufferNum;
	int BufferNum;
	int readBytes;
	long file_size;
	long totalReadBytes;


	char buf[BUF_SIZE];

	FILE * fp;
	fp = fopen(argv[2], "wb");

	s_accept = accept(s_listen, (SOCKADDR *)&client_addr, &len_addr);
	if (s_accept) {
		
		printf("Connection Request from Client [IP:%s, Port:%d] has been Accepted\n",
			inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		readBytes = recv(s_accept, buf, BUF_SIZE, 0);
		file_size = atol(buf);
		totalBufferNum = file_size / BUF_SIZE + 1;
		BufferNum = 0;
		totalReadBytes = 0;
		start = GetMicroCounter();
		while (BufferNum != totalBufferNum) {
			readBytes = recv(s_accept, buf, BUF_SIZE, 0);
			BufferNum++;
			totalReadBytes += readBytes;
			//printf("In progress: %d/%dByte(s) [%d%%]\n", totalReadBytes, file_size, ((BufferNum * 100) / totalBufferNum));
			fwrite(buf, sizeof(char), readBytes, fp);

			if (readBytes == SOCKET_ERROR) {
				printf("File Receive Errpr");
				exit(1);
			}

		}
		end = GetMicroCounter();
		closesocket(s_accept);
		
		//printf("time: %d second(s)", end - start);
		std::cout << "time : " << (end - start) / 1000 << "ms" << std::endl;
	}
	else {
		printf("File Accept Error");
	}

	closesocket(s_listen);
	WSACleanup();

	return 0;
}

u64 GetMicroCounter()
{
	u64 Counter;

#if defined(_WIN32)
	u64 Frequency;
	QueryPerformanceFrequency((LARGE_INTEGER *)&Frequency);
	QueryPerformanceCounter((LARGE_INTEGER *)&Counter);
	Counter = 1000000 * Counter / Frequency;
#elif defined(__linux__)
	struct timeval t;
	gettimeofday(&t, 0);
	Counter = 1000000 * t.tv_sec + t.tv_usec;
#endif

	return Counter;
}
#else
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <chrono>
typedef unsigned long long u64;
u64 GetMicroCounter();

using namespace boost::asio::ip;

int main(int argc, char **argv) {
	boost::asio::io_context ioc;
	boost::asio::ssl::context ctx_(boost::asio::ssl::context::tlsv13_server);

	ctx_.set_options(
		boost::asio::ssl::context::default_workarounds
		| boost::asio::ssl::context::no_sslv2
		| boost::asio::ssl::context::no_sslv3
		| boost::asio::ssl::context::no_tlsv1_1
		| boost::asio::ssl::context::single_dh_use
	);
	boost::system::error_code ec;
	ctx_.use_certificate_chain_file("server.pem", ec);
	if (ec) {
		std::cout << 
			"Load certificat Failed (" << ec.message() << ")" << std::endl;
		return false;
	}
	ctx_.use_private_key_file("server.key", boost::asio::ssl::context::pem, ec);
	if (ec) {
		std::cout << 
			"Load private_key Failed(" << ec.message() << ")" << std::endl;
		return false;
	}
	ctx_.use_tmp_dh_file("dh2048.pem", ec);
	if (ec) {
		std::cout <<
			"Load tmp_dh Failed(" << ec.message() << ")" << std::endl;
		return false;
	}

	u64 start, end;
	u64 start1, end1;
	u64 start2, end2;
	u64 testset1 = 0;
	u64 testset = 0;

	if (argc != 3) {
		printf("Command parameter does not right. \n<port> <filename>\n");
		exit(1);
	}

	tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));
	tcp::acceptor acceptor(ioc, endpoint);
	
	tcp::socket socket(ioc);

	int totalBufferNum;
	int BufferNum;
	int readBytes = 0;
	long file_size;
	long totalReadBytes;

	char buf[BUF_SIZE];

	FILE * fp;
	fp = fopen(argv[2], "wb");

	acceptor.accept(socket);

	socket.set_option(tcp::no_delay(true));

	boost::asio::ssl::stream<tcp::socket> sslSocket(std::move(socket), ctx_);

	sslSocket.handshake(boost::asio::ssl::stream_base::server, ec);

	start = GetMicroCounter();
	printf("Connection Request from Client\n");
	
	readBytes = boost::asio::read(sslSocket, boost::asio::buffer(buf, BUF_SIZE));
	file_size = atol(buf);
	totalBufferNum = file_size / BUF_SIZE + 1;
	BufferNum = 0;
	totalReadBytes = 0;
	boost::system::error_code err;
	start = GetMicroCounter();
	std::chrono::system_clock::time_point chronostart = std::chrono::system_clock::now();

	while (BufferNum != totalBufferNum) {
		start1 = GetMicroCounter();
		readBytes = boost::asio::read(sslSocket, boost::asio::buffer(buf, BUF_SIZE),err);
		end1 = GetMicroCounter();
		testset += end1 - start1;
		//if (err) {
		//	printf("File Receive Errpr \n");
		//	std::cerr << "message : " << err.message() << std::endl;
		//	exit(1);
		//}
		BufferNum++;
		totalReadBytes += readBytes;
		//printf("In progress: %d/%dByte(s) [%d%%]\n", totalReadBytes, file_size, ((BufferNum * 100) / totalBufferNum));
		start2 = GetMicroCounter();
		fwrite(buf, sizeof(char), readBytes, fp);
		end2 = GetMicroCounter();
		testset1 += end2 - start2;
	}

	std::chrono::system_clock::time_point chronoend = std::chrono::system_clock::now();

	std::chrono::milliseconds mill
		= std::chrono::duration_cast<std::chrono::milliseconds>(chronoend - chronostart);
	std::chrono::seconds sec
		= std::chrono::duration_cast<std::chrono::seconds>(chronoend - chronostart);
	std::chrono::minutes min
		= std::chrono::duration_cast<std::chrono::minutes>(chronoend - chronostart);
	std::cout << "time : " << mill.count() << " milliseconds" << std::endl;
	std::cout << "time : " << sec.count() << " seconds" << std::endl;
	std::cout << "time : " << min.count() << " minutes" << std::endl;

	//end = GetMicroCounter();
	////printf("time: %f second(s)", (end - start)/10000);
	////std::cout << "time : " << (end - start) / 1000 << "ms" << std::endl;
	//printf("\nElapsed Time (micro seconds) : %lld", end - start);
	//printf("\nElapsed Time (micro seconds) : %lld\n", testset);
	//printf("\nElapsed Time (micro seconds) : %lld\n", testset1);

	getchar();

	socket.close();
	return 0;
}


u64 GetMicroCounter()
{
	u64 Counter;
	u64 Frequency;
	QueryPerformanceFrequency((LARGE_INTEGER *)&Frequency);
	QueryPerformanceCounter((LARGE_INTEGER *)&Counter);
	Counter = 1000000 * Counter / Frequency;

	return Counter;
}
#endif // !1
