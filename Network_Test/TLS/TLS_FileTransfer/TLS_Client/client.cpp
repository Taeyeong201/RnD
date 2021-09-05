#define BUF_SIZE 4096

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>

typedef unsigned long long u64;
u64 GetMicroCounter();

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
		/*tls_stream.lowest_layer().set_option(
			boost::asio::ip::tcp::no_delay(true));*/

	}

	void close() {
		boost::system::error_code ec;

		tls_stream.shutdown(ec);

		tls_stream.lowest_layer().shutdown(
			boost::asio::ip::tcp::socket::shutdown_both, ec);

		tls_stream.lowest_layer().close(ec);
	}
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket>* get_TLS_Stream() {
		return &tls_stream;
	};
private:
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
};

int main(int argc, char **argv) {
	u64 start, end;
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
	//sendBytes = socket.send(boost::asio::buffer(buf, BUF_SIZE));
	auto stream = tlsSocket.get_TLS_Stream();
	sendBytes = boost::asio::write(*stream, boost::asio::buffer(buf, BUF_SIZE));

	start = GetMicroCounter();
	while ((sendBytes = fread(buf, sizeof(char), sizeof(buf), fp)) > 0) {
		boost::asio::write(*stream, boost::asio::buffer(buf, BUF_SIZE));
		BufferNum++;
		totalSendBytes += sendBytes;
		//printf("In progress: %d/%dByte(s) [%d%%]\n", totalSendBytes,
		//	file_size, ((BufferNum * 100) / totalBufferNum));
	}
	end = GetMicroCounter();
	//printf("time: %f second(s)", (end - start)/10000);
	std::cout << "time : " << (end - start) / 1000 << "ms" << std::endl;

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