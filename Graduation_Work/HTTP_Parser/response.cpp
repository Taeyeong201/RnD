#include <iostream>


#include <string>
#include <vector>
#include <sstream>
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>

//#include "http.h"

// directly embed the source here
//extern "C" {
//#include "http.c"
//#include "header.c"
//#include "chunk.c"
//}

//#include <httpparser/response.h>
//#include <httpparser/httpresponseparser.h>

#include "json\json.h"
//#pragma  comment(lib,"jsoncpp\\lib\\jsoncpp.lib")

int main(int, char**)
{
    bool needmore = true;
    char buffer[4096];
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
        "ETag: W/\"3f-UsEBvOeFDxsdwPNckyzRAeb85PE\"\r\n"
        "Date: Sat, 13 Jun 2020 20:07:54 GMT\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "{\"id\":1,\"email\":\"219.248.240.15\",\"name\":\"STpuncher\",\"active\":1}";


    std::vector<std::string> httpHeader;
    std::string strBuffer(text1);
    std::istringstream input(text1);
    std::string::size_type n;
    int lineCount = 0;
    for (std::string line; std::getline(input, line); lineCount++) {
        //n = line.find("\r");
        //std::cout << "found: " << line.substr(n) << '\n';
        //line.erase(line.find("\r"));

        std::cout << line.find("\r") << std::endl;
        if (line.size() > line.find("\r"))
            line.pop_back();

        if (lineCount == 0 && 0 != line.compare("HTTP/1.1 200 OK")) {
            std::cout << "Response Error" << std::endl;
            return 0;
        }


        httpHeader.push_back(line);

    }
    for (std::string test : httpHeader) {
        std::cout << test << std::endl;
    }

    std::string json = httpHeader.back();

    JSONCPP_STRING err;
    Json::Value root;

    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(json.c_str(), json.c_str() + json.length(), &root,
        &err)) {
        std::cout << "error" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << root["id"].asString() << std::endl;
    std::cout << root["email"] << std::endl;
    std::cout << root["name"] << std::endl;
    std::cout << root["active"] << std::endl;

    //Json::Value family = root["family"];
    //std::cout << family[0].asString() << std::endl;
    //std::cout << family[1].asString() << std::endl;
    //std::cout << family[2].asString() << std::endl;


}
