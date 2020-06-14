#include "Puncher.h"

using namespace ST;

Puncher::Puncher(const char* serverName, uint16_t portNum)
	:ip(serverName), port(portNum)
{

};

Puncher::~Puncher() {

};

bool Puncher::connectToServer() {
	struct hostent* hp;
	unsigned int addr;
	struct sockaddr_in server;

	this->conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->conn == INVALID_SOCKET)
		return false;

	//if (inet_addr(szServerName) == INADDR_NONE)
	//{
	//    hp = gethostbyname(szServerName);
	//    server.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
	//    server.sin_family = AF_INET;
	//    server.sin_port = htons(portNum);
	//}
	//else
	{
		addr = inet_addr(ip.c_str());
		server.sin_addr.s_addr = addr;
		server.sin_family = AF_INET;
		server.sin_port = htons(port);
	}

	if (connect(this->conn, (struct sockaddr*)&server, sizeof(server)))
	{
		closesocket(this->conn);
		return false;
	}


	return true;
}

void ST::Puncher::start()
{
	punchThread = std::thread(&Puncher::run, this);
}

void ST::Puncher::stop()
{
	sendDataQueuing("exit");
	punchThread.join();
}

void ST::Puncher::sendServer(const std::string& data)
{
	const char* _data = data.c_str();
	memset(buffer, 0, 4096);
	sprintf(buffer, post_str.c_str(),
		filePath.c_str(),
		ip.c_str(), port,
		strlen(_data),
		_data
	);
	send(this->conn, buffer, strlen(buffer), 0);
	std::cout << "=============== sending ===============\n" <<
		buffer << "\n=======================================\n" << std::endl;


}

int ST::Puncher::getHeaderLength(char* content)
{
	const char* srchStr1 = "\r\n\r\n", * srchStr2 = "\n\r\n\r";
	char* findPos;
	int ofset = -1;

	findPos = strstr(content, srchStr1);
	if (findPos != NULL)
	{
		ofset = findPos - content;
		ofset += strlen(srchStr1);
	}

	else
	{
		findPos = strstr(content, srchStr2);
		if (findPos != NULL)
		{
			ofset = findPos - content;
			ofset += strlen(srchStr2);
		}
	}
	return ofset;
}


std::string ST::Puncher::recvServer()
{
	std::string data;
	int flag = 0;
	while (1) {
		if (flag == 500) {
			std::cout << "recv time out" << std::endl;
			return std::string();
		}
		mutex_recv.lock();
		if (dataRecvQueue.empty()) {
			mutex_recv.unlock();
			flag++;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}
		data = std::string(dataRecvQueue.front());
		dataRecvQueue.pop();
		mutex_recv.unlock();

		break;
	}

	return data;
}

void ST::Puncher::sendDataQueuing(const std::string data)
{
	std::unique_lock<std::mutex> lk(mutex_send);
	this->dataSendQueue.push(data);
	lk.unlock();
	cv_send.notify_one();
}

void ST::Puncher::recvDataQueuing(const std::string data)
{
	std::unique_lock<std::mutex> lk(mutex_recv);
	this->dataRecvQueue.push(data);
	lk.unlock();
}


void ST::Puncher::run()
{
	std::string data;
	long recv_len;
	long totalBytesRead, headerLen;
	//char* result;

	if (!connectToServer()) {
		std::cout << "Connection ERR" << std::endl;
		std::cout << "Puncher Thread exit" << std::endl;
		return;
	}

	while (1) {
		std::unique_lock<std::mutex> lk(mutex_send);
		cv_send.wait(lk, [this]() {
			//std::cout << "wait" << std::endl;
			return !dataSendQueue.empty();
		});

		data = std::string(dataSendQueue.front());
		dataSendQueue.pop();
		lk.unlock();

		if (strcmp(data.c_str(), "exit") == 0) {
			std::cout << "Puncher Thread exit" << std::endl;
			return;
		}

		sendServer(data);


		memset(recvBuffer, 0, 4096);
		recv_len = recv(conn, recvBuffer, 4096, 0);
		if (recv_len < 0) {
			std::cout << "Puncher Thread recv error" << std::endl;
		}
		else {
			recvBuffer[recv_len] = '\0';
			recvDataQueuing(std::string(recvBuffer));
		}


		//headerLen = getHeaderLength(tmpResult);
		//long contenLen = totalBytesRead - headerLen;
		//result = new char[contenLen + 1];
		//memcpy(result, tmpResult + headerLen, contenLen);
		//result[contenLen] = 0x0;
		//char* myTmp;

		//myTmp = new char[headerLen + 1];
		//strncpy(myTmp, tmpResult, headerLen);
		//myTmp[headerLen] = NULL;
		//delete(tmpResult);
		//*headerOut = myTmp;

		//bytesReturnedOut = contenLen;
		//closesocket(conn);

	}

	return;
}
