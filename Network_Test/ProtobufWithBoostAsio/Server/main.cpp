#define _WIN32_WINNT 0x0A00

//ref : https://stackoverflow.com/questions/37950139/writing-a-simple-c-protobuf-streaming-client-server

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "ProtobufHelpers.h"
#include "AsioAdapting.h"
#include "addressbook.pb.h"

#include "testMsg.pb.h"

using boost::asio::ip::tcp;

bool LoadCerificate(boost::asio::ssl::context& ctx);

int main()
{
	boost::asio::io_service io_service;
	boost::asio::ssl::context ctx(boost::asio::ssl::context::tls_server);

	if (!LoadCerificate(ctx)) return -1;

	tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 27015));

	for (;;)
	{
		boost::system::error_code ec;
		tcp::socket socket(io_service);
		acceptor.accept(socket);
		boost::asio::ssl::stream<tcp::socket> scSocket(std::move(socket), ctx);
		scSocket.handshake(boost::asio::ssl::stream_base::server, ec);

		AsioOutputStream<boost::asio::ssl::stream<tcp::socket>> aos(scSocket); // Where m_Socket is a instance of boost::asio::ip::tcp::socket
		CopyingOutputStreamAdaptor cos_adp(&aos);
		int i = 0;
		std::vector<std::future<void>> threads;
		do {
			++i;

			tutorial::TestMsg testmsg;

			switch (i % 3) {
			case 0: 
			{
				tutorial::Test1* test = testmsg.mutable_test1();
				test->set_data(i);
			}
				break;
			case 1:
			{
				tutorial::Test2* test = testmsg.mutable_test2();
				test->set_text("test2");
			}
				break;
			case 2:
			{
				tutorial::Test3* test = testmsg.mutable_test3();
				test->set_text("test3");
			}
				break;
			default:
				break;
			}

			
			//myMessage.set_id(i);
			google::protobuf::io::writeDelimitedTo(testmsg, &cos_adp);
			// Now we have to flush, otherwise the write to the socket won't happen until enough bytes accumulate
			cos_adp.Flush();

		} while (i < 5000);

	}
	return 0;
}

bool LoadCerificate(boost::asio::ssl::context& ctx) {
	boost::system::error_code ec;

	ctx.set_options(
		boost::asio::ssl::context::default_workarounds
		| boost::asio::ssl::context::no_sslv2
		| boost::asio::ssl::context::no_sslv3
		| boost::asio::ssl::context::no_tlsv1_1
		| boost::asio::ssl::context::single_dh_use
	);
	ctx.use_certificate_chain_file("server.pem", ec);
	if (ec) {
		std::cout << "Load certificat Failed (" << ec.message() << ")" << std::endl;
		return false;
	}
	ctx.use_private_key_file("server.key", boost::asio::ssl::context::pem, ec);
	if (ec) {
		std::cout << "Load private_key Failed(" << ec.message() << ")" << std::endl;
		return false;
	}
	ctx.use_tmp_dh_file("dh2048.pem", ec);
	if (ec) {
		std::cout << "Load tmp_dh Failed(" << ec.message() << ")" << std::endl;
		return false;
	}

	return true;
}