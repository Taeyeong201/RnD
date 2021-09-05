#define BUF_SIZE 4096
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <WinSock2.h>
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "Ws2_32.lib")
typedef unsigned long long u64;
u64 GetMicroCounter();

//파라메터 :  <server IP> <port> <filename>
int main(int argc, char **argv) {
	u64 start, end;
	WSADATA wsaData;
	struct sockaddr_in server_addr;
	SOCKET s;

	if (argc != 4) {
		printf("Command parameter does not right. \n<server IP> <port> <filename>\n");
		exit(1);
	}

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket Creat Error.\n");
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));

	if (connect(s, (SOCKADDR *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		printf("Socket Connection Error.\n");
		exit(1);
	}

	printf("File Send Start\n");

	int totalBufferNum;
	int BufferNum;
	int sendBytes;
	long totalSendBytes;
	long file_size;
	char buf[BUF_SIZE];

	FILE *fp;
	fp = fopen(argv[3], "rb");
	if (fp == NULL) {
		printf("File not Exist");
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	totalBufferNum = file_size / sizeof(buf) + 1;
	fseek(fp, 0, SEEK_SET);
	BufferNum = 0;
	totalSendBytes = 0;

	_snprintf(buf, sizeof(buf), "%d", file_size);
	sendBytes = send(s, buf, sizeof(buf), 0);

	start = GetMicroCounter();
	while ((sendBytes = fread(buf, sizeof(char), sizeof(buf), fp)) > 0) {
		send(s, buf, sendBytes, 0);
		BufferNum++;
		totalSendBytes += sendBytes;
		//printf("In progress: %d/%dByte(s) [%d%%]\n", totalSendBytes, file_size, ((BufferNum * 100) / totalBufferNum));
	}
	end = GetMicroCounter();
	//printf("time: %d second(s)", end - start);
	std::cout << "time : " << (end - start) / 1000 << "ms" << std::endl;

	closesocket(s);
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
#include <stdlib.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>
#include <chrono>

typedef unsigned long long u64;
u64 GetMicroCounter();

using namespace boost::asio::ip;

bool callbackVerifyCertificate(
	bool preverified, boost::asio::ssl::verify_context& contxet)
{
	char subject_name[256];
	X509* cert = X509_STORE_CTX_get_current_cert(contxet.native_handle());
	X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
	std::cout << "Verifying " << subject_name << "\n";

	return preverified;
}

int main(int argc, char **argv) {
	u64 start, end;
	u64 start1, end1;
	u64 testset = 0;
	boost::asio::io_context ioc;
	boost::asio::ssl::context ctx_(boost::asio::ssl::context::tlsv13_client);

	boost::system::error_code ec;
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> tls_socket_(ioc, ctx_);

	ctx_.load_verify_file("rootCA.pem", ec);
	if (ec) {
		std::cout << "load verify file failed(" << ec.message() << ")"
			<<std::endl;
		return false;
	}
	tls_socket_.set_verify_mode(boost::asio::ssl::verify_peer);
	tls_socket_.set_verify_callback(
		boost::bind(&callbackVerifyCertificate, _1, _2));

	if (argc != 4) {
		printf("Command parameter does not right. \n<server IP> <port> <filename>\n");
		exit(1);
	}

	tcp::endpoint endpoint(address::from_string(argv[1]), atoi(argv[2]));

	//tcp::socket socket(ioc);

	tls_socket_.lowest_layer().connect(endpoint);
	tls_socket_.lowest_layer().set_option(tcp::no_delay(true));

	tls_socket_.handshake(boost::asio::ssl::stream_base::client, ec);
	if (ec) {
		std::cout
			<< "Handshake Failed(" << ec.message() << ")"
			<< std::endl;
		return false;
	}


	printf("File Send Start\n");

	int totalBufferNum;
	int BufferNum;
	int sendBytes;
	long totalSendBytes;
	long file_size;
	char buf[BUF_SIZE];

	FILE *fp;
	fp = fopen(argv[3], "rb");
	if (fp == NULL) {
		printf("File not Exist");
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	totalBufferNum = file_size / sizeof(buf) + 1;
	fseek(fp, 0, SEEK_SET);
	BufferNum = 0;
	totalSendBytes = 0;

	_snprintf(buf, sizeof(buf), "%d", file_size);
	//sendBytes = send(s, buf, sizeof(buf), 0);
	sendBytes = boost::asio::write(tls_socket_,boost::asio::buffer(buf, BUF_SIZE));
	std::chrono::system_clock::time_point chronostart = std::chrono::system_clock::now();

	start = GetMicroCounter();
	while ((sendBytes = fread(buf, sizeof(char), sizeof(buf), fp)) > 0) {
		start1 = GetMicroCounter();
		sendBytes = boost::asio::write(tls_socket_, boost::asio::buffer(buf, BUF_SIZE));
		end1 = GetMicroCounter();
		testset += end1 - start1;
		BufferNum++;
		totalSendBytes += sendBytes;
		//printf("In progress: %d/%dByte(s) [%d%%]\n", totalSendBytes, 
			//file_size, ((BufferNum * 100) / totalBufferNum));
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
	//printf("\nElapsed Time (micro seconds) : %lld\n", end - start);
	//printf("Elapsed Time (micro seconds) : %lld\n", testset);
	getchar();

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
#endif