#include <iostream>

#include <httpparser/response.h>
#include <httpparser/httpresponseparser.h>

using namespace httpparser;

int main(int, char**)
{
    const char text[] =
            "HTTP/1.1 200 OK\r\n"
            "Server: nginx/1.2.1\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 8\r\n"
            "Connection: keep-alive\r\n"
            "\r\n"
            "<html />";
    const char text1[] =
        "HTTP/1.1 200 OK\r\n"
        "X-Powered-By: Express\r\n"
        "Content-Type: application/json; charset=utf-8\r\n"
        "Content-Length: 63\r\n"
        "ETag: W/""3f - UsEBvOeFDxsdwPNckyzRAeb85PE""\r\n"
        "Date: Sat, 13 Jun 2020 20:07:54 GMT\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "{""id"":1,""email"":""219.248.240.15"",""name"":""STpuncher"",""active"":1}";

    Response response;
    HttpResponseParser parser;

    HttpResponseParser::ParseResult res = parser.parse(response, text1, text1 + sizeof(text1));

    if( res == HttpResponseParser::ParsingCompleted )
    {
        std::cout << response.inspect() << std::endl;
        return EXIT_SUCCESS;
    }
    else
    {
        std::cerr << "Parsing failed" << std::endl;
        return EXIT_FAILURE;
    }
}
