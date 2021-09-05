#define BUF_SIZE 4096
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>
typedef unsigned long long u64;
u64 GetMicroCounter();

using namespace boost::asio::ip;
class TLS_SockServerStream {
public:
	TLS_SockServerStream() :
		tls_context(boost::asio::ssl::context::tlsv13_server)
	{

	};
	void create_TLS_Stream(boost::asio::ip::tcp::socket& sock) {
		socket_ =
			std::make_shared<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>
			(std::move(sock), tls_context);
	}
	void setSecurity() {
		tls_setOption();
		verify_certificate();
	};

	std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> socket_;

private:
	void tls_setOption() {
		tls_context.set_options(
			boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::no_sslv2
			| boost::asio::ssl::context::no_sslv3
			| boost::asio::ssl::context::no_tlsv1_1
			//| boost::asio::ssl::context::
			| boost::asio::ssl::context::single_dh_use);
	};
	void verify_certificate() {
		tls_context.use_certificate_chain_file("root.pem");
		tls_context.use_private_key_file("root.key", boost::asio::ssl::context::pem);
		tls_context.use_tmp_dh_file("dh2048.pem");
	};
	boost::asio::ssl::context tls_context;
};


int main(int argc, char **argv) {
	boost::asio::io_context ioc;
	u64 start, end;

	if (argc != 3) {
		printf("Command parameter does not right. \n<port> <filename>\n");
		exit(1);
	}

	tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));
	tcp::acceptor acceptor(ioc, endpoint);
	tcp::socket socket(ioc);
	TLS_SockServerStream tlsSock;


	int totalBufferNum;
	int BufferNum;
	int readBytes = 0;
	long file_size;
	long totalReadBytes;

	char buf[BUF_SIZE];

	FILE * fp;
	fp = fopen(argv[2], "wb");
	tlsSock.setSecurity();
	acceptor.accept(socket);

	tlsSock.create_TLS_Stream(socket);
	tlsSock.socket_->handshake(boost::asio::ssl::stream_base::server);
	//tlsSock.socket_->lowest_layer().set_option(boost::asio::ip::tcp::no_delay(true));
	start = GetMicroCounter();
	printf("Connection Request from Client\n");

	readBytes = boost::asio::read(*(tlsSock.socket_), boost::asio::buffer(buf, BUF_SIZE));
	file_size = atol(buf);
	totalBufferNum = file_size / BUF_SIZE + 1;
	BufferNum = 0;
	totalReadBytes = 0;
	boost::system::error_code err;
	start = GetMicroCounter();
	while (BufferNum != totalBufferNum) {
		readBytes = boost::asio::read(*(tlsSock.socket_), boost::asio::buffer(buf, BUF_SIZE), err);
		//readBytes = boost::asio::read(socket, boost::asio::buffer(buf, BUF_SIZE), err);
		//if (err) {
		//	printf("File Receive Errpr \n");
		//	std::cerr << "message : " << err.message() << std::endl;
		//	exit(1);
		//}
		BufferNum++;
		totalReadBytes += readBytes;
		//printf("In progress: %d/%dByte(s) [%d%%]\n", totalReadBytes, file_size, ((BufferNum * 100) / totalBufferNum));
		fwrite(buf, sizeof(char), readBytes, fp);
	}

	end = GetMicroCounter();
	//printf("time: %f second(s)", (end - start)/10000);
	std::cout << "time : " << (end - start) / 1000 << "ms" << std::endl;

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