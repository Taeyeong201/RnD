//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//#include <WinSock2.h>
//#include <Windows.h>

#include <cstdlib>
#include <functional>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

using boost::asio::ip::tcp;
using namespace boost::asio;
char data_[1024];

void initTLS(ssl::context& context_) {
	context_.set_options(
		boost::asio::ssl::context::default_workarounds
		| boost::asio::ssl::context::no_sslv2
		| boost::asio::ssl::context::no_sslv3
		| boost::asio::ssl::context::no_tlsv1_1
		//| boost::asio::ssl::context::
		| boost::asio::ssl::context::single_dh_use);

	context_.use_certificate_chain_file("root.pem");
	context_.use_private_key_file("root.key", boost::asio::ssl::context::pem);
	context_.use_tmp_dh_file("dh2048.pem");
}

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 2)
		{
			std::cerr << "Usage: server <port>\n";
			return 1;
		}

		io_context ioc;

		ssl::context tls_context(ssl::context::tlsv13_server);

		ip::tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));
		ip::tcp::acceptor acceptor(ioc, endpoint);
		ip::tcp::socket socket(ioc);

		initTLS(tls_context);

		while (1) {
			acceptor.accept(socket);
			boost::system::error_code err;

			boost::asio::ssl::stream<tcp::socket> socket_(std::move(socket), tls_context);
			socket_.handshake(boost::asio::ssl::stream_base::server);

			socket_.read_some(buffer(data_, sizeof(data_)), err);
			if (err) {
				std::cout << "err : " << err.message() << std::endl;
				break;
			}
			std::cout << "클라이언트 : " << data_ << std::endl;

			//socket_.write_some(buffer(data_, strlen(data_)), err);
			boost::asio::write(socket_, buffer(data_, strlen(data_)), err);
			if (err) {
				std::cout << "1err : " << err.message() << std::endl;
				break;
			}
			std::cout << "보낸 메시지 : " << data_ << std::endl;
		}
		

	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}