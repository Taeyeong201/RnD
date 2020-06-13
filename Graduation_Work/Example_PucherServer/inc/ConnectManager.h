#pragma once
#include "st_common.h"

namespace ST {

	class ST_DLL ConnectManager
	{
    public:
        ConnectManager(SOCKET sockfd = -1);
        virtual ~ConnectManager();

        virtual SOCKET create();

        virtual bool bind(std::string ip, uint16_t port);
        virtual bool listen(int backlog);
        virtual SOCKET accept();

        virtual bool connect(std::string ip, uint16_t port, int timeout = 0);

        virtual void close();
        virtual void shutdownWrite();

        SOCKET fd() const { return _sockfd; }

    private:
        SOCKET _sockfd = -1;
	};

};

