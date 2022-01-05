#define _WIN32_WINNT 0x0A00

//ref : https://stackoverflow.com/questions/37950139/writing-a-simple-c-protobuf-streaming-client-server

#include <boost/asio.hpp>
#include "ProtobufHelpers.h"
#include "AsioAdapting.h"
#include "addressbook.pb.h"

using boost::asio::ip::tcp;
int main()
{
    const char* hostname = "127.0.0.1";
    const char* port = "27015";
    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(hostname, port);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);
    AsioInputStream<tcp::socket> ais(socket);
    CopyingInputStreamAdaptor cis_adp(&ais);
    for (;;)
    {
        tutorial::Person myMessage;
        if (!google::protobuf::io::readDelimitedFrom(&cis_adp, &myMessage)) break;
        std::cout << "id : " << myMessage.id() << std::endl;
    }
    return 0;
}