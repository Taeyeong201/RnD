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


std::string ST::Puncher::responseServerRawData()
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
DWORD convert_unicode_to_utf8_string(
	__out std::string& utf8,
	__in const wchar_t* unicode,
	__in const size_t unicode_size
) {
	DWORD error = 0;
	do {
		if ((nullptr == unicode) || (0 == unicode_size)) {
			error = ERROR_INVALID_PARAMETER;
			break;
		}
		utf8.clear();
		//
		// getting required cch.
		//
		int required_cch = ::WideCharToMultiByte(
			CP_UTF8,
			WC_ERR_INVALID_CHARS,
			unicode, static_cast<int>(unicode_size),
			nullptr, 0,
			nullptr, nullptr
		);
		if (0 == required_cch) {
			error = ::GetLastError();
			break;
		}
		//
		// allocate.
		//
		utf8.resize(required_cch);
		//
		// convert.
		//
		if (0 == ::WideCharToMultiByte(
			CP_UTF8,
			WC_ERR_INVALID_CHARS,
			unicode, static_cast<int>(unicode_size),
			const_cast<char*>(utf8.c_str()), static_cast<int>(utf8.size()),
			nullptr, nullptr
		)) {
			error = ::GetLastError();
			break;
		}
	} while (false);
	return error;
}

std::vector<std::string> ST::Puncher::responseHttpParseData(const std::string &raw)
{
	int lineCount = 0;
	std::vector<std::string> httpHeader;
	std::istringstream input(raw);
	std::string::size_type n;
	for (std::string line; std::getline(input, line); lineCount++) {
		//n = line.find("\r");
		//std::cout << "found: " << line.substr(n) << '\n';
		//line.erase(line.find("\r"));
		if (line.size() > line.find("\r"))
			line.pop_back();

		if (lineCount == 0 && 0 != line.compare("HTTP/1.1 200 OK")) {
			std::cout << "Response Error" << std::endl;
		}
		httpHeader.push_back(line);

	}


	JSONCPP_STRING err;
	std::string json = httpHeader.back();
	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(json.c_str(), json.c_str() + json.length(), &jsonRoot,
		&err)) {
		std::cout << "json parsing error" << std::endl;
	}
	std::cout << "=====json=====" << std::endl;
	std::cout << jsonRoot["id"].asString() << std::endl;
	std::cout << jsonRoot["email"] << std::endl;
	std::cout << jsonRoot["name"].asString() << std::endl;
	std::cout << jsonRoot["active"] << std::endl;
	std::cout << "=====json=====" << std::endl;

	return httpHeader;
}

std::string ST::Puncher::getParseData(const std::string &id)
{
	return  jsonRoot[id].asString();
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
	std::string sendData;
	long recv_len;
	long totalBytesRead, headerLen;
	//char* result;

	while (1) {
		if (!connectToServer()) {
			std::cout << "Connection ERR" << std::endl;
			std::cout << "Puncher Thread exit" << std::endl;
			return;
		}

		std::unique_lock<std::mutex> lk(mutex_send);
		cv_send.wait(lk, [this]() {
			//std::cout << "wait" << std::endl;
			return !dataSendQueue.empty();
		});

		sendData = std::string(dataSendQueue.front());
		dataSendQueue.pop();
		lk.unlock();

		if (strcmp(sendData.c_str(), "exit") == 0) {
			std::cout << "Puncher Thread exit" << std::endl;
			closesocket(this->conn);
			return;
		}

		sendServer(sendData);


		memset(recvBuffer, 0, 4096);
		recv_len = recv(conn, recvBuffer, 4096, 0);
		if (recv_len < 0) {
			std::cout << "Puncher Thread recv error" << std::endl;
			return;
		}
		else {
			recvBuffer[recv_len] = '\0';
		}
		recvDataQueuing(std::string(recvBuffer));

		closesocket(this->conn);
	}

	return;
}
