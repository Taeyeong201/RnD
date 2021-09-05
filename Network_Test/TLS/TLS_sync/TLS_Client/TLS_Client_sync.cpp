#include <WinSock2.h>
#include <Windows.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>


enum { max_length = 1024 };
using namespace boost;

class TLS_Stream {
public:
	TLS_Stream(const std::string& raw_ip_address,
		unsigned short port_num) :
		endpoint(asio::ip::address::from_string(raw_ip_address),port_num),
		ctx(asio::ssl::context::tlsv13_client),
		tls_stream(ioc, ctx)
	{
		ctx.load_verify_file("user.pem");
		tls_stream.set_verify_mode(asio::ssl::verify_peer);
		tls_stream.set_verify_callback(
			boost::bind(&TLS_Stream::verify_certificate, this, _1, _2));
	}

	void connect() {
		tls_stream.lowest_layer().connect(endpoint);
		tls_stream.lowest_layer().set_option(asio::ip::tcp::no_delay(true));

		tls_stream.handshake(asio::ssl::stream_base::client);
	}

	void close() {
		boost::system::error_code ec;

		tls_stream.shutdown(ec);

		tls_stream.lowest_layer().shutdown(
			boost::asio::ip::tcp::socket::shutdown_both, ec);

		tls_stream.lowest_layer().close(ec);
	}
	void write(char *buf, size_t size) {
		asio::write(tls_stream, asio::buffer(buf, size));
	}
	void read(char *buf, size_t size, boost::system::error_code& err) {
		//tls_stream.read_some(asio::buffer(buf, size), err);
		asio::read(tls_stream, asio::buffer(buf, size), err);
	}

private:
	bool verify_certificate(bool preverified,
		asio::ssl::verify_context& contxet)
	{
		char subject_name[256];
		X509* cert = X509_STORE_CTX_get_current_cert(contxet.native_handle());
		X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
		std::cout << "Verifying " << subject_name << "\n";

		return preverified;
	}

	void send_request(const std::string& request) {
		asio::write(tls_stream, asio::buffer(request));
	}

	std::string receive_response() {
		asio::streambuf buf;
		asio::read_until(tls_stream, buf, '\n');

		std::string response;
		std::istream input(&buf);
		std::getline(input, response);

		return response;
	}

private:
	asio::io_service ioc;
	asio::ssl::context ctx;
	asio::ip::tcp::endpoint endpoint;
	asio::ssl::stream<asio::ip::tcp::socket> tls_stream;
};

char data_[1024];

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 3)
		{
			std::cerr << "Usage: client <host> <port>\n";
			return 1;
		}

		TLS_Stream stream(argv[1], std::atoi(argv[2]));

		stream.connect();
		//stream.close();
		std::cout << "메세지 입력 : ";
		std::cin.getline(data_, max_length);
		stream.write(data_, sizeof(data_));
		
		system::error_code err;
		memset(data_, 0, sizeof(data_));

		stream.read(data_, sizeof(data_), err);
		if (err) {
			std::cerr << "read err :" << err.message() << std::endl;
		}
		std::cout << "서버 : " << data_ << std::endl;

	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
