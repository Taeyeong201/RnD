//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <functional>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/thread/thread.hpp>

using boost::asio::ip::tcp;

char aa[1024] = "asdasdasdasd";

/*
class session : public std::enable_shared_from_this<session>
{
public:
	session(tcp::socket socket, boost::asio::ssl::context& context)
		: socket_(std::move(socket), context)
	{
		
	}

	void start()
	{
		do_handshake();
	}

	void do_write(std::size_t length)
	{
		auto self(shared_from_this());
		boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
			[this, self](const boost::system::error_code& ec,
				std::size_t)
		{
			if (!ec)
			{
				do_read();
			}
		});
	}

private:
	void do_handshake()
	{
		auto self(shared_from_this());
		socket_.async_handshake(boost::asio::ssl::stream_base::server,
			[this, self](const boost::system::error_code& error)
		{
			if (!error)
			{
				do_read();
			}
		});
	}

	void do_read()
	{
		auto self(shared_from_this());
		socket_.async_read_some(boost::asio::buffer(data_),
			[this, self](const boost::system::error_code& ec, std::size_t length)
		{
			if (!ec)
			{
				std::cout << "Receied: ";
				std::cout.write(data_, length);
				std::cout << "\n";
				std::cout << "Sending back again\n";


				do_write(length);
			}
		});
	}

	boost::asio::ssl::stream<tcp::socket> socket_;
	char data_[1024];
};
*/
// boost::asio::ssl::context ctx(io_service,boost::asio::ssl::context_base::sslv3_client);
class server
{
public:
	server(boost::asio::io_context& io_context, unsigned short port)
		: acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
		//context_(boost::asio::ssl::context::sslv23)
		context_(boost::asio::ssl::context::tlsv13_server)
	{
		context_.set_options(
			boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::no_sslv2
			| boost::asio::ssl::context::no_sslv3
			| boost::asio::ssl::context::no_tlsv1_1
			//| boost::asio::ssl::context::
			| boost::asio::ssl::context::single_dh_use);
		//context_.set_password_callback(std::bind(&server::get_password, this));
		context_.use_certificate_chain_file("server.pem");
		context_.use_private_key_file("server.key", boost::asio::ssl::context::pem);
		context_.use_tmp_dh_file("dh2048.pem");

		do_accept();
	}
	void start()
	{
		do_handshake();
	}

	void do_write(char *data__, std::size_t length)
	{
		socket_->async_write_some(boost::asio::buffer(data__, length),
			[this](const boost::system::error_code& ec, std::size_t length)
		{
			if (!ec)
			{
				std::cout << "Sending back again\n";
				//do_read();
			}
			else {
				std::cout << std::string(ec.message()) << std::endl;
			}
		});
	}
	void do_read()
	{
		socket_->async_read_some(boost::asio::buffer(data_),
			[this](const boost::system::error_code& ec, std::size_t length)
		{
			if (!ec)
			{
				std::cout << "Receied: ";
				std::cout.write(data_, length);
				std::cout << "\n";

				/*boost::asio::async_write(socket_, boost::asio::buffer(aa, sizeof(aa)), boost::bind(asdasd));*/
				//do_write(length);
			}
			else {
				std::cout << std::string(ec.message()) << std::endl;
			}
		});
	}
private:
	std::string get_password() const
	{
		return "test";
	}

	void do_accept()
	{
		acceptor_.async_accept(
			[this](const boost::system::error_code& error, tcp::socket socket)
		{
			if (!error)
			{
				socket_ = std::make_shared<boost::asio::ssl::stream<tcp::socket>>(std::move(socket), context_);
				do_handshake();
			}
		});
	}
	void do_handshake()
	{

		socket_->async_handshake(boost::asio::ssl::stream_base::server,
			[this](const boost::system::error_code& error)
		{
			if (!error)
			{
				do_read();
			}
		});
	}



	std::shared_ptr<boost::asio::ssl::stream<tcp::socket>> socket_;
	char data_[1024];
	tcp::acceptor acceptor_;
	boost::asio::ssl::context context_;
};

boost::asio::io_context io_context;
server* servertest;
void test() {
	using namespace std; // For atoi.
	servertest = new server(io_context, 9000);
	io_context.run();
}

int main(int argc, char* argv[])
{
	try
	{
		/*if (argc != 2)
		{
			std::cerr << "Usage: server <port>\n";
			return 1;
		}*/

		auto th = boost::thread::thread(test);
		std::string input_string;
		while (true) 
		{
			std::cin >> input_string;
			if (input_string.compare("exit") == 0) {
				break;
			}
			else if (input_string.compare("send") == 0) {
				servertest->do_write(aa, 100);
				//break;
			}
			else if (input_string.compare("read") == 0) {
				//servertest->do_read();

			}
			input_string.clear();
		}
		io_context.stop();
		if (th.joinable()) {
			th.join();
		}

		//auto worker = new boost::asio::io_context::work(io_context);
		//delete worker;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}