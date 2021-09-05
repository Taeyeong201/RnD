#define BUF_SIZE 4096
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>
typedef unsigned long long u64;
u64 GetMicroCounter();
u64 start, end;
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
	void setSecurity(char *filename) {
		tls_setOption();
		verify_certificate();

		fp = fopen(filename, "wb");
		if (fp == NULL) {
			printf("File not Exist");
			exit(1);
		}
	};
	void readFileSize() {
		std::cout << "connect" << std::endl;
		boost::asio::async_read(*socket_,
			boost::asio::buffer(buf, BUF_SIZE),
			boost::bind(&TLS_SockServerStream::readFileSizeHandle, this));
	}
	std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> socket_;

private:
	void readFileSizeHandle() {
		file_size = atol(buf);
		totalBufferNum = file_size / BUF_SIZE + 1;
		BufferNum = 0;
		totalReadBytes = 0;
		boost::asio::async_read(*socket_,
			boost::asio::buffer(buf, BUF_SIZE),
			boost::bind(&TLS_SockServerStream::readFileData, this,
				boost::asio::placeholders::bytes_transferred));
		start = GetMicroCounter();
	}
	void readFileData(size_t bytes_transferred) {
		if (BufferNum != totalBufferNum) {
			BufferNum++;
			totalReadBytes += bytes_transferred;
			fwrite(buf, sizeof(char), bytes_transferred, fp);
			boost::asio::async_read(*socket_,
				boost::asio::buffer(buf, BUF_SIZE),
				boost::bind(&TLS_SockServerStream::readFileData, this,
					boost::asio::placeholders::bytes_transferred));
		}
		else {
			end = GetMicroCounter();
			//printf("time: %f second(s)", (end - start)/10000);
			std::cout << "time : " << (end - start) / 1000 << "ms" << std::endl;
			fclose(fp);
		}
	}
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


	int totalBufferNum;
	int BufferNum;
	int readBytes = 0;
	long file_size;
	long totalReadBytes;

	char buf[BUF_SIZE];

	FILE * fp;
};


int main(int argc, char **argv) {
	boost::asio::io_context ioc;
	

	if (argc != 3) {
		printf("Command parameter does not right. \n<port> <filename>\n");
		exit(1);
	}

	tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));
	tcp::acceptor acceptor(ioc, endpoint);
	tcp::socket socket(ioc);
	TLS_SockServerStream tlsSock;
	
	tlsSock.setSecurity(argv[2]);
	acceptor.accept(socket);

	tlsSock.create_TLS_Stream(socket);
	tlsSock.socket_->handshake(boost::asio::ssl::stream_base::server);

	tlsSock.readFileSize();
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