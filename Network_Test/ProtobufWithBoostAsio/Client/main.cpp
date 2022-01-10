#define _WIN32_WINNT 0x0A00

//ref : https://stackoverflow.com/questions/37950139/writing-a-simple-c-protobuf-streaming-client-server

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>
#include "ProtobufHelpers.h"
#include "AsioAdapting.h"
#include "addressbook.pb.h"
#include "testMsg.pb.h"

using boost::asio::ip::tcp;

bool CallbackVerifyCertificate(
	bool preverified, boost::asio::ssl::verify_context& contxet);

int main()
{
	const char* hostname = "127.0.0.1";
	const char* port = "27015";
	boost::system::error_code ec;
	boost::asio::io_service io_service;
	boost::asio::ssl::context ctx(boost::asio::ssl::context::tls_client);
	ctx.load_verify_file("rootCA.pem", ec);


	tcp::resolver resolver(io_service);
	tcp::resolver::query query(hostname, port);
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

	boost::asio::ssl::stream<tcp::socket> scSocket(io_service, ctx);
	scSocket.set_verify_mode(boost::asio::ssl::verify_peer);
	scSocket.set_verify_callback(
		boost::bind(&CallbackVerifyCertificate, _1, _2));

	scSocket.lowest_layer().connect(*endpoint_iterator, ec);
	//boost::asio::connect(,);

	scSocket.handshake(boost::asio::ssl::stream_base::client, ec);
	if (ec)
		return -1;

	AsioInputStream<boost::asio::ssl::stream<tcp::socket>> ais(scSocket);
	CopyingInputStreamAdaptor cis_adp(&ais);
	for (;;)
	{
		//tutorial::Person myMessage;
		tutorial::TestMsg testMsg;
		if (!google::protobuf::io::readDelimitedFrom(&cis_adp, &testMsg)) break;
		switch (testMsg.WhatTest_case()) {
		case tutorial::TestMsg::WhatTestCase::kTest1:
			std::cout << "test1 : " << testMsg.test1().data() << std::endl;
			break;
		case tutorial::TestMsg::WhatTestCase::kTest2:
			std::cout << "test2 : " << testMsg.test2().text() << std::endl;
			break;
		case tutorial::TestMsg::WhatTestCase::kTest3:
			std::cout << "test3 : " << testMsg.test3().text() << std::endl;
			break;
		default:
			break;
		}
	}
	return 0;
}

bool CallbackVerifyCertificate(bool preverified,
	boost::asio::ssl::verify_context& contxet) {
	char subject_name[256];
	X509* cert = X509_STORE_CTX_get_current_cert(contxet.native_handle());
	X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
	std::cout << "Verifying " << subject_name << "\n";

	return preverified;
}