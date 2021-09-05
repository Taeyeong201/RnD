#include <cstdlib>
#include <cstring>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/thread/thread.hpp>

char buffer[1024] = { 0, };


class TLS_AsyncSocket {
public:
	TLS_AsyncSocket(

		boost::asio::io_context& ioc) :
		ctx(boost::asio::ssl::context::tlsv13_client),
		tls_stream(ioc, ctx)
	{
		ctx.load_verify_file("user.pem");
		tls_stream.set_verify_mode(boost::asio::ssl::verify_peer);
		tls_stream.set_verify_callback(
			boost::bind(&TLS_AsyncSocket::verify_certificate, this, _1, _2));
	};
	void connect(boost::asio::ip::tcp::endpoint& address) {
		boost::system::error_code err;
		tls_stream.lowest_layer().connect(address);

		tls_stream.handshake(boost::asio::ssl::stream_base::client, err);
		if (err) {
			std::cerr << "HandShake ERR" << std::endl;
		}
		tls_stream.lowest_layer().set_option(
			boost::asio::ip::tcp::no_delay(true));

	}

	void close() {
		boost::system::error_code ec;

		tls_stream.shutdown(ec);

		//tls_stream.lowest_layer().shutdown(
		//	boost::asio::ip::tcp::socket::shutdown_both, ec);

		tls_stream.lowest_layer().close(ec);
	}

	void error_control(
		const char* __file, const char* __func, const int line, const char* err_name, const int mslm_code,
		const boost::system::error_code& error) {
		if (error) {
			printf("-- ERROR -- error_code >> %d \t message >> %s\nfile : %s, func : %s, line : %d, name : %s\n",
				error.value(), error.message(), __file, __func, line, err_name);
		}
		else {
			if (mslm_code == 1) {
				printf("-- read OK -- \nfile : %s, func : %s, line : %d, name : %s\n",
					__file, __func, line, err_name);
				std::cout << buffer << std::endl;
			}
			else {
				printf("-- send OK -- \nfile : %s, func : %s, line : %d, name : %s\n",
					__file, __func, line, err_name);
				std::cout << buffer << std::endl;
			}
		}
	}
	void test_error(const boost::system::error_code& error) {
		std::cout << "handler in" << std::endl;
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
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> tls_stream;
};


boost::asio::io_context io_context;
TLS_AsyncSocket asyncSocket(io_context);

void test() {
	io_context.run();
}

int main(int argc, char* argv[]) {
	try
	{
		/*if (argc != 2)
		{
			std::cerr << "Usage: server <port>\n";
			return 1;
		}*/
		boost::asio::ip::tcp::endpoint server(boost::asio::ip::address::from_string(argv[1]), std::atoi(argv[2]));
		asyncSocket.connect(server);

		//char* tes111 = "zxczxczxczxczxczxczxc123456;";
		//strcpy(buffer, tes111);
		//asyncSocket.get_TLS_Stream()->async_write_some(boost::asio::buffer(buffer, sizeof(buffer)),
		//	boost::bind(&TLS_AsyncSocket::error_control, boost::ref(asyncSocket),
		//		__FILE__, __FUNCTION__, __LINE__, "test", 2, boost::asio::placeholders::error));

		//asyncSocket.get_TLS_Stream()->async_read_some(boost::asio::buffer(buffer, sizeof(buffer)),
		//	boost::bind(&TLS_AsyncSocket::error_control, boost::ref(asyncSocket),
		//		__FILE__, __FUNCTION__, __LINE__, "test", 1, boost::asio::placeholders::error));

		boost::thread::thread(&test);

		//auto worker = std::make_shared<boost::asio::io_context::work>(io_context);
		std::string input_string;
		while (true)
		{
			std::cin >> input_string;
			if (input_string.compare("exit") == 0) {
				break;
			}
			else if (input_string.compare("read") == 0) {
				asyncSocket.get_TLS_Stream()->async_read_some(boost::asio::buffer(buffer, sizeof(buffer)),
					boost::bind(&TLS_AsyncSocket::error_control, boost::ref(asyncSocket), 
						__FILE__, __FUNCTION__, __LINE__, "test", 1, boost::asio::placeholders::error));
				//break;
			}
			else if (input_string.compare("send") == 0) {
				char* tes111 = "zxczxczxczxczxczxczxc123456;";
				strcpy(buffer, tes111);
				asyncSocket.get_TLS_Stream()->async_write_some(boost::asio::buffer(buffer, sizeof(buffer)),
					boost::bind(&TLS_AsyncSocket::test_error, boost::ref(asyncSocket),boost::asio::placeholders::error));

			}
			input_string.clear();
		}

		//worker = nullptr;

		//auto worker = new boost::asio::io_context::work(io_context);
		//delete worker;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
}