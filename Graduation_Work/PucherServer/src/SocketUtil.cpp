#include "SocketUtil.h"

int ST::SocketUtil::SHUT_RD = 0;
int ST::SocketUtil::SHUT_WR = 1;
int ST::SocketUtil::SHUT_RDWR = 2;

bool ST::SocketUtil::bind(SOCKET sockfd, std::string ip, uint16_t port)
{
    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);

    if (::bind(sockfd, (struct sockaddr*) & addr, sizeof addr) == SOCKET_ERROR)
    {
        return false;
    }

    return true;
}

void ST::SocketUtil::setNonBlock(SOCKET fd)
{
#if defined(__linux) || defined(__linux__) 
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    unsigned long on = 1;
    ioctlsocket(fd, FIONBIO, &on);
#endif
}

void ST::SocketUtil::setBlock(SOCKET fd, int writeTimeout)
{
#if defined(__linux) || defined(__linux__) 
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags & (~O_NONBLOCK));
#elif defined(WIN32) || defined(_WIN32)
    unsigned long on = 0;
    ioctlsocket(fd, FIONBIO, &on);
#else
#endif
    if (writeTimeout > 0)
    {
#ifdef SO_SNDTIMEO
#if defined(__linux) || defined(__linux__) 
        struct timeval tv = { writeTimeout / 1000, (writeTimeout % 1000) * 1000 };
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof tv);
#elif defined(WIN32) || defined(_WIN32)
        unsigned long ms = (unsigned long)writeTimeout;
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&ms, sizeof(unsigned long));
#else
#endif
#endif
    }
}

void ST::SocketUtil::setReuseAddr(SOCKET sockfd)
{
    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof on);
}

void ST::SocketUtil::setReusePort(SOCKET sockfd)
{
#ifdef SO_REUSEPORT
    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&on, sizeof(on));
#endif
}

void ST::SocketUtil::setNoDelay(SOCKET sockfd)
{
#ifdef TCP_NODELAY
    int on = 1;
    int ret = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on));
#endif
}

void ST::SocketUtil::setKeepAlive(SOCKET sockfd)
{
    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char*)&on, sizeof(on));
}

void ST::SocketUtil::setNoSigpipe(SOCKET sockfd)
{
#ifdef SO_NOSIGPIPE
    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE, (char*)&on, sizeof(on));
#endif
}

void ST::SocketUtil::setSendBufSize(SOCKET sockfd, int size)
{
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char*)&size, sizeof(size));
}

void ST::SocketUtil::setRecvBufSize(SOCKET sockfd, int size)
{
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char*)&size, sizeof(size));
}

void ST::SocketUtil::setRecvTimeout(SOCKET sockfd, int timeout)
{
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
}

std::string ST::SocketUtil::getPeerIp(SOCKET sockfd)
{
    struct sockaddr_in addr = { 0 };
    socklen_t addrlen = sizeof(struct sockaddr_in);
    if (getpeername(sockfd, (struct sockaddr*) & addr, &addrlen) == 0)
    {
        return inet_ntoa(addr.sin_addr);
    }

    return "0.0.0.0";
}

uint16_t ST::SocketUtil::getPeerPort(SOCKET sockfd)
{
    struct sockaddr_in addr = { 0 };
    socklen_t addrlen = sizeof(struct sockaddr_in);
    if (getpeername(sockfd, (struct sockaddr*) & addr, &addrlen) == 0)
    {
        return ntohs(addr.sin_port);
    }

    return 0;
}

int ST::SocketUtil::getPeerAddr(SOCKET sockfd, struct sockaddr_in* addr)
{
    socklen_t addrlen = sizeof(struct sockaddr_in);
    return getpeername(sockfd, (struct sockaddr*)addr, &addrlen);
}

void ST::SocketUtil::close(SOCKET sockfd)
{
#if defined(__linux) || defined(__linux__) 
    ::close(sockfd);
#elif defined(WIN32) || defined(_WIN32)
    ::closesocket(sockfd);
#endif
}

bool ST::SocketUtil::connect(SOCKET sockfd, std::string ip, uint16_t port, int timeout)
{
    bool isConnected = true;
    if (timeout > 0)
    {
        ST::SocketUtil::setNonBlock(sockfd);
    }

    struct sockaddr_in addr = { 0 };
    socklen_t addrlen = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (::connect(sockfd, (struct sockaddr*) & addr, addrlen) == SOCKET_ERROR)
    {
        if (timeout > 0)
        {
            isConnected = false;
            fd_set fdWrite;
            FD_ZERO(&fdWrite);
            FD_SET(sockfd, &fdWrite);
            struct timeval tv = { timeout / 1000, timeout % 1000 * 1000 };
            select((int)sockfd + 1, NULL, &fdWrite, NULL, &tv);
            if (FD_ISSET(sockfd, &fdWrite))
            {
                isConnected = true;
            }
            SocketUtil::setBlock(sockfd);
        }
        else
        {
            isConnected = false;
        }
    }

    return isConnected;
}


std::string ST::SocketUtil::GetLocalIPAddress()
{
#if defined(__linux) || defined(__linux__) 
    SOCKET sockfd = 0;
    char buf[512] = { 0 };
    struct ifconf ifconf;
    struct ifreq* ifreq;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET)
    {
        close(sockfd);
        return "0.0.0.0";
    }

    ifconf.ifc_len = 512;
    ifconf.ifc_buf = buf;
    if (ioctl(sockfd, SIOCGIFCONF, &ifconf) < 0)
    {
        close(sockfd);
        return "0.0.0.0";
    }

    close(sockfd);

    ifreq = (struct ifreq*)ifconf.ifc_buf;
    for (int i = (ifconf.ifc_len / sizeof(struct ifreq)); i > 0; i--)
    {
        if (ifreq->ifr_flags == AF_INET)
        {
            if (strcmp(ifreq->ifr_name, "lo") != 0)
            {
                return inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr);
            }
            ifreq++;
        }
    }
    return "0.0.0.0";
#elif defined(WIN32) || defined(_WIN32)
    PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
    unsigned long size = sizeof(IP_ADAPTER_INFO);

    int ret = GetAdaptersInfo(pIpAdapterInfo, &size);
    if (ret == ERROR_BUFFER_OVERFLOW)
    {
        delete pIpAdapterInfo;
        pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[size];
        ret = GetAdaptersInfo(pIpAdapterInfo, &size);
    }

    if (ret != ERROR_SUCCESS)
    {
        delete pIpAdapterInfo;
        return "0.0.0.0";
    }

    while (pIpAdapterInfo)
    {
        IP_ADDR_STRING* pIpAddrString = &(pIpAdapterInfo->IpAddressList);
        while (pIpAddrString)
        {
            if (strcmp(pIpAddrString->IpAddress.String, "127.0.0.1") != 0
                && strcmp(pIpAddrString->IpAddress.String, "0.0.0.0") != 0)
            {
                // pIpAddrString->IpMask.String 
                //pIpAdapterInfo->GatewayList.IpAddress.String
                std::string ip(pIpAddrString->IpAddress.String);
                //delete pIpAdapterInfo;
                return ip;
            }
            pIpAddrString = pIpAddrString->Next;
        } while (pIpAddrString);
        pIpAdapterInfo = pIpAdapterInfo->Next;
    }

    delete pIpAdapterInfo;
    return "0.0.0.0";
#else
    return "0.0.0.0";
#endif
}
