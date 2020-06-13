#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")

#define BUFF_SIZE 1024
char* http_write_buf = "POST /%s HTTP/1.1\r\n"
"Host: %s:%d\r\n"
"Connection: keep-alive\r\n"
"Content-Length: %d\r\n"
"User-Agent: Hanilab RX\r\n"
"Cache-Control: no-cache\r\n"
"Origin: hanilab-rx\r\n"
"Content-Type: application/json\r\n"
"Accept: */*\r\n"
"Accept-Encoding: gzip, deflate, br\r\n"
"Accept-Language: ko-KR,ko;q=0.8,en-US;q=0.6,en;q=0.4\r\n"
"\r\n"
"%s";

char* post_str = "GET /customers/1 HTTP/1.1\r\n"
"Host: 192.168.0.23:60001\r\n"
"Content-Type: application/json\r\n"
//"Content-Type: text/plain; charset=urf-8\r\n"
"Content-Length: %d\r\n"
//"Accept-Language: ko-KR,ko;q=0.8,en-US;q=0.6,en;q=0.4\r\n"
"Accept: */*\r\n"
"Accept-Encoding: gzip, deflate, br\r\n"
"Connection: keep-alive\r\n"
"\r\n"
"%s";
char* str1 =
"{\n"
"\"name\": \"Hyun\",\n"
"\"age\": \"24\",\n"
"\"addr\": \"JeJu\",\n"
"\"tel\": \"010-222-3333\"\n"
"}";
char total_str[1024];
int main()
{
	WSADATA wsa;
	struct hostent* host;
	char msg[BUFF_SIZE]; //나중에 결과값을 저장하기 위한 선언

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		perror("WSAStart Error ");
		system("pause");
		return -1;
	}

	SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addr;

	if (sock == INVALID_SOCKET)
	{
		perror("Sock Error ");
		system("pause");
		return -1;
	}
	//host = gethostbyname("192.168.0.23");

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3001);
	addr.sin_addr.s_addr = inet_addr("219.248.240.15");/*inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));*/

	if (connect(sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		printf("Not Connect \n");
		system("pause");
		return 1;
	}
	//char* str = "POST /member HTTP/1.1\r\nHost: 192.168.0.23:60001\r\n\r\n";
	sprintf(total_str, post_str, strlen(str1), str1);
	printf("%s \n\n\n", total_str);
	send(sock, total_str, strlen(total_str), 0);
	int rc;
	rc = recv(sock, msg, BUFF_SIZE, 0);
	msg[rc] = '\0';

	printf("%s \n", msg);

	printf("end\n");
	closesocket(sock);
	WSACleanup();
	system("pause");

	return 0;
}