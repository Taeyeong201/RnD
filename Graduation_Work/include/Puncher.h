#pragma once

#include "st_common.h"

#include "json/json.h"

#include <queue>
#include <thread>
#include <functional>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <sstream>

namespace ST {

	class ST_DLL Puncher
	{
	public:
		Puncher(const char* serverName, uint16_t portNum);
		~Puncher();

		void start();
		void stop();

		void sendDataQueuing(const std::string data);
		std::string responseServerRawData();
		std::vector<std::string> responseHttpParseData(const std::string &raw);
		std::string getParseData(const std::string &id);


	private:
		void run();
		bool connectToServer();
		void sendServer(const std::string &data);
		int getHeaderLength(char* content);
		void recvDataQueuing(const std::string data);

		char buffer[4096] = { 0, };
		char recvBuffer[4096] = { 0, };
		SOCKET conn = 0;

		Json::Value jsonRoot;

		std::thread punchThread;
		std::mutex mutex_send;
		std::mutex mutex_recv;
		std::condition_variable cv_send;  
		//std::condition_variable cv_recv;  
		std::queue<std::string> dataSendQueue;
		std::queue<std::string> dataRecvQueue;


		std::string ip;
		unsigned short port;

		std::string filePath = "/customers/4";
		std::string post_str = "GET %s HTTP/1.1\r\n"
			"Host: %s:%d\r\n"
			"Content-Type: application/json; charset=utf-8\r\n"
			//"Content-Type: text/plain; charset=urf-8\r\n"
			"Content-Length: %d\r\n"
			//"Accept-Language: ko-KR,ko;q=0.8,en-US;q=0.6,en;q=0.4\r\n"
			"Accept: */*\r\n"
			"Accept-Encoding: gzip, deflate, br\r\n"
			"Connection: keep-alive\r\n"
			"\r\n"
			"%s";
	};
}

