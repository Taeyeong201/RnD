#include <iostream>

#include "st_common.h"
#include "ConnectManager.h"
#include "SocketUtil.h"
#include "Puncher.h"

#pragma comment(lib, "Ws2_32.lib")

#define TASK_SCHEDULER_PRIORITY_LOW       0
#define TASK_SCHEDULER_PRIORITY_NORMAL    1
#define TASK_SCHEDULER_PRIORITYO_HIGH     2
#define TASK_SCHEDULER_PRIORITY_HIGHEST   3
#define TASK_SCHEDULER_PRIORITY_REALTIME  4

#define RECV_TEST
#define SEND_TEST

int main() {
	// Load Windows Socket DLL
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		std::cerr << "Win socket start failed" << std::endl;
		WSACleanup();
	}

	// Get console handle
	HWND hConsole = GetConsoleWindow();

	// Set Thread Priority
	int priority = TASK_SCHEDULER_PRIORITY_REALTIME;

	SetThreadDescription(GetCurrentThread(), L"main_thread");

	switch (priority)
	{
	case TASK_SCHEDULER_PRIORITY_LOW:
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
		break;
	case TASK_SCHEDULER_PRIORITY_NORMAL:
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
		break;
	case TASK_SCHEDULER_PRIORITYO_HIGH:
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
		break;
	case TASK_SCHEDULER_PRIORITY_HIGHEST:
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
		break;
	case TASK_SCHEDULER_PRIORITY_REALTIME:
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
		break;
	}

	bool bResult = false;
	int ret = 0;
	//std::cout << "local ip : " << ip << std::endl;

	char* sendbuf = (char*)malloc(512);
	char* recvbuf = (char*)malloc(512);

	size_t sendlen = _msize(sendbuf);
	size_t recvlen = _msize(recvbuf);

	uint16_t port = 9000;
	std::string ip = ST::SocketUtil::GetLocalIPAddress();
	char buffer[2048];
	char *str1 =
		"{\n"
		"\"ip\": \"%s\",\n"
		"\"project\": \"UserServer\",\n"
		"\"state\": \"Waiting\",\n"
		"}";

	ST::Puncher STPuncher("192.168.0.28", 3001);
	STPuncher.start();
	sprintf(buffer, str1, ip.c_str());
	STPuncher.sendDataQueuing(buffer);
	STPuncher.stop();



	std::shared_ptr<ST::ConnectManager> tcpSocket(new ST::ConnectManager);
	SOCKET clientSocketfd = -1;

	tcpSocket->create();
	//SocketUtil::setReuseAddr(tcpSocket->fd());
	//SocketUtil::setReusePort(tcpSocket->fd());
	//SocketUtil::setNonBlock(tcpSocket->fd());
	//SocketUtil::setKeepAlive(tcpSocket->fd());

	//bResult = tcpSocket->bind(ip, port);
	bResult = tcpSocket->bind(port);
	if (FALSE == bResult)
	{
		std::cerr << "main tcpSocket bind failed" << std::endl;
		std::cerr << "GetLastError : " << WSAGetLastError() << std::endl;

		goto EXIT;
	}

	bResult = tcpSocket->listen(SOMAXCONN);
	if (FALSE == bResult)
	{
		std::cerr << "main tcpSocket listen failed" << std::endl;
		std::cerr << "GetLastError : " << WSAGetLastError() << std::endl;

		goto EXIT;
	}

	std::cout << "Listening........" << std::endl;


	clientSocketfd = tcpSocket->accept();
	if (clientSocketfd <= 0)
	{
		std::cerr << "main tcpSocket accept failed" << std::endl;
		std::cerr << "GetLastError : " << WSAGetLastError() << std::endl;

		goto EXIT;
	}

	std::cout << "Accept OK!" << std::endl;

#ifdef SEND_TEST

	strcpy(sendbuf, "Hello Im TX, This is send test bye!\n");

	// TODO : What is diff send return values
	ret = ::send(clientSocketfd, (char*)sendbuf, sendlen, 0);
	if (ret > 0)
	{
		std::cout << "Send OK - sendsize: " << ret << std::endl;
	}
	else if (ret < 0)
	{
		std::cout << "Send ret < 0" << std::endl;
#if defined(__linux) || defined(__linux__)
		if (errno == EINTR || errno == EAGAIN)
#elif defined(WIN32) || defined(_WIN32)
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK || error == WSAEINPROGRESS || error == 0)
#endif
			ret = 0;
	}
	else
	{
		std::cout << "Send ret = 0" << std::endl;
	}
#endif

#ifdef RECV_TEST
	ret = ::recv(clientSocketfd, (char*)recvbuf, recvlen, 0);
	if (ret == -1)
	{
		// Error Handling;
		std::cout << "Error Recv return -1, Handling Please" << std::endl;
		goto EXIT;
	}
	else if (ret == 0)
	{
		std::cout << "Recv return 0, This socket is dead. close this socket" << std::endl;
		goto EXIT;
	}
	else // Recv OK
	{
		//if (ret < wantrecvsize) // This case can not receive hole data
		//{
		//      // Return Get Data
		//      return ret;
		//}
		//else
		std::cout << "All Received!" << std::endl;

		std::cout << "[RECV message] " << recvbuf << std::endl;
	}
#endif

EXIT:
	free(sendbuf);
	free(recvbuf);

	ST::SocketUtil::close(clientSocketfd);
	tcpSocket->close();

	// Unload Windows Socket DLL
	WSACleanup();

	return 0;
};