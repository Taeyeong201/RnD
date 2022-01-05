#define _WIN32_WINNT 0x0A00

//ref : https://stackoverflow.com/questions/37950139/writing-a-simple-c-protobuf-streaming-client-server

#include <boost/asio.hpp>
#include "ProtobufHelpers.h"
#include "AsioAdapting.h"
#include "addressbook.pb.h"

using boost::asio::ip::tcp;
int main()
{
	boost::asio::io_service io_service;
	tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 27015));
	for (;;)
	{
		tcp::socket socket(io_service);
		acceptor.accept(socket);
		AsioOutputStream<boost::asio::ip::tcp::socket> aos(socket); // Where m_Socket is a instance of boost::asio::ip::tcp::socket
		CopyingOutputStreamAdaptor cos_adp(&aos);
		int i = 0;
		std::vector<std::future<void>> threads;
		do {
			++i;

			tutorial::Person myMessage;
			myMessage.set_id(i);
			google::protobuf::io::writeDelimitedTo(myMessage, &cos_adp);
			// Now we have to flush, otherwise the write to the socket won't happen until enough bytes accumulate
			cos_adp.Flush();

		} while (i < 5000);

	}
	return 0;
}