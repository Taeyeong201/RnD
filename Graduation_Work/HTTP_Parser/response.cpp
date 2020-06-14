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
        "ETag: W/""3f - UsEBvOeFDxsdwPNckyzRAeb85PE""\r\n"
        "Date: Sat, 13 Jun 2020 20:07:54 GMT\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "{""id"":1,""email"":""219.248.240.15"",""name"":""STpuncher"",""active"":1}";


    std::vector<std::string> httpHeader;
    strcpy(buffer, text1);
    std::string strBuffer(text1);
    std::istringstream input(text1);
    std::string::size_type n;
    for (std::string line; std::getline(input, line);) {
        //n = line.find("\r");
        //std::cout << "found: " << line.substr(n) << '\n';
        //line.erase(line.find("\r"));
        std::cout << line.find("\r") << std::endl;
        if(line.size() > line.find("\r"))
            line.pop_back();
        httpHeader.push_back(line);

    }
    for (std::string test : httpHeader) {
        std::cout << test << std::endl;
    }

    //buffer = (char*)malloc(4096);

    //strcpy(buffer, text1);

    //int ndata = strlen(text1);
    //HttpResponse response;
    //response.code = 0;

    //http_roundtripper rt;
    //http_init(&rt, responseFuncs, &response);

    //while (needmore && ndata) {
    //    int read;
    //    needmore = http_data(&rt, buffer, ndata, &read);
    //    ndata -= read;
    //    buffer += read;
    //}
    //if (http_iserror(&rt)) {
    //    fprintf(stderr, "Error parsing data\n");
    //    http_free(&rt);

    //    return -1;
    //}

    //http_free(&rt);

    //printf("Response: %d\n", response.code);
    //if (!response.body.empty()) {
    //    printf("%s\n", &response.body[0]);
    //}
}
//
//    Response response;
//    HttpResponseParser parser;
//
//    HttpResponseParser::ParseResult res = parser.parse(response, text1, text1 + sizeof(text1));
//
//    if( res == HttpResponseParser::ParsingCompleted )
//    {
//        std::cout << response.inspect() << std::endl;
//        return EXIT_SUCCESS;
//    }
//    else
//    {
//        std::cerr << "Parsing failed" << std::endl;
//        return EXIT_FAILURE;
//    }
//}
