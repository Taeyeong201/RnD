#include "ConnectManager.h"

#include "SocketUtil.h"
#include <iostream>

#if defined(WIN32) || defined(_WIN32) 
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib")
#endif 



ST::ConnectManager::ConnectManager(SOCKET sockfd)
    : _sockfd(sockfd)
{
}

ST::ConnectManager::~ConnectManager()
{
}

SOCKET ST::ConnectManager::create()
{
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    return _sockfd;
}

bool ST::ConnectManager::bind(std::string ip, uint16_t port)
{
    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);

    if (::bind(_sockfd, (struct sockaddr*) & addr, sizeof(addr)) == SOCKET_ERROR)
    {
        std::cerr << "socket = " << _sockfd
            << " bind failed - IP : " << ip.c_str() << " Port : " << port << std::endl;

        return false;
    }

    return true;
}

bool ST::ConnectManager::bind(uint16_t port)
{
    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (::bind(_sockfd, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        std::cerr << "socket = " << _sockfd
            << " bind failed - " << " Port : " << port << std::endl;

        return false;
    }

    return true;
    return false;
}

bool ST::ConnectManager::listen(int backlog)
{
    if (::listen(_sockfd, backlog) == SOCKET_ERROR)
    {
        std::cerr << "socket = " << _sockfd
            << "listen failed" << std::endl;

        return false;
    }

    return true;
}

SOCKET ST::ConnectManager::accept()
{
    struct sockaddr_in addr = { 0 };
    socklen_t addrlen = sizeof addr;

    SOCKET clientfd = ::accept(_sockfd, (struct sockaddr*) & addr, &addrlen);

    return clientfd;
}

bool ST::ConnectManager::connect(std::string ip, uint16_t port, int timeout)
{
    if (!SocketUtil::connect(_sockfd, ip, port, timeout))
    {
        std::cerr << "socket = " << _sockfd
            << " connect failed" << std::endl;

        return false;
    }

    return true;
}

void ST::ConnectManager::close()
{
    closesocket(_sockfd);

    _sockfd = 0;
}

void ST::ConnectManager::shutdownWrite()
{
    shutdown(_sockfd, SocketUtil::SHUT_RD);
    _sockfd = 0;
}