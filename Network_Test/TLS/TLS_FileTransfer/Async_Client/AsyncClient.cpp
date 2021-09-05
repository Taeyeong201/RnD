#define BUF_SIZE 4096

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>

typedef unsigned long long u64;
u64 GetMicroCounter();
u64 start, end;
using namespace boost::asio::ip;

class TLS_SockClientStream {
public:
	TLS_SockClientStream(

		boost::asio::io_context& ioc) :
		ctx(boost::asio::ssl::context::tlsv13_client),
		tls_stream(ioc, ctx)
	{
		ctx.load_verify_file("user.pem");
		tls_stream.set_verify_mode(boost::asio::ssl::verify_peer);
		tls_stream.set_verify_callback(
			boost::bind(&TLS_SockClientStream::verify_certificate, this, _1, _2));
	};
	void connect(boost::asio::ip::tcp::endpoint& servAddr_AUDIO_) {
		boost::system::error_code err;
		tls_stream.lowest_layer().connect(servAddr_AUDIO_);

		tls_stream.handshake(boost::asio::ssl::stream_base::client, err);
		if (err) {
			std::cerr << "HandShake ERR" << std::endl;
		}
	}

	void setting(char *filename) {
		fp = fopen(filename, "rb");
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
	}

	void sendFile() {
		start = GetMicroCounter();
		boost::asio::async_write(tls_stream, boost::asio::buffer(buf, BUF_SIZE),
			boost::bind(&TLS_SockClientStream::sendFileHandle, this,
				boost::asio::placeholders::bytes_transferred));
	}

	boost::asio::ssl::stream<boost::asio::ip::tcp::socket>* get_TLS_Stream() {
		return &tls_stream;
	};
private:
	void sendFileHandle(size_t bytes_transferred) {
		sendBytes = bytes_transferred;
		boost::asio::async_write(tls_stream, boost::asio::buffer(buf, BUF_SIZE),
			boost::bind(&TLS_SockClientStream::sendFileHandle2, this));
	}
	void sendFileHandle2() {
		if ((sendBytes = fread(buf, sizeof(char), sizeof(buf), fp)) > 0) {
			boost::asio::async_write(tls_stream, boost::asio::buffer(buf, BUF_SIZE),
				boost::bind(&TLS_SockClientStream::sendFileHandle2, this));
			BufferNum++;
			totalSendBytes += sendBytes;
		}
		else {
			end = GetMicroCounter();
			std::cout << "time : " << (end - start) / 1000 << "ms" << std::endl;
		}
	}
	bool verify_certificate(bool preverified,
		boost::asio::ssl::verify_context& contxet)
	{
		char subject_name[256];
		X509* cert = X509_STORE_CTX_get_current_cert(contxet.native_handle());
		X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
		std::cout << "Verifying " << subject_name << "\n";

		return preverified;
	}

	boost::asio::ssl::context ctx;
	tcp::endpoint* servAddr_AUDIO;

	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> tls_stream;
	int totalBufferNum;
	int BufferNum;
	int sendBytes;
	long totalSendBytes;
	long file_size;
	FILE *fp;
	char buf[BUF_SIZE];
};

int main(int argc, char **argv) {

	boost::asio::io_context ioc;

	if (argc != 4) {
		printf("Command parameter does not right. \n<server IP> <port> <filename>\n");
		exit(1);
	}

	tcp::endpoint endpoint(address::from_string(argv[1]), atoi(argv[2]));

	//tcp::socket socket(ioc);

	//socket.connect(endpoint);
	TLS_SockClientStream tlsSocket(ioc);
	tlsSocket.connect(endpoint);

	printf("File Send Start");

	tlsSocket.setting(argv[3]);
	tlsSocket.sendFile();
	ioc.run();


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