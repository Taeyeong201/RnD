#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>
#include <wchar.h>
#pragma comment(lib,"ws2_32")

SOCKET sockfd_gui;
SOCKET sockfd_gui_client;
char buffer[1024];

int main() {
	struct sockaddr_in gui_addr;
	WSADATA guiData;
	WSAStartup(MAKEWORD(2, 2), &guiData);
	sockfd_gui = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd_gui < 0) {
		perror("ERROR opening GUI socket");
		return -1;
	}
	struct sockaddr_in guiAddr = { 0 };
	guiAddr.sin_family = AF_INET;
	guiAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	guiAddr.sin_port = htons(9000);
	if (bind(sockfd_gui, (sockaddr*)&guiAddr, sizeof(guiAddr)) == SOCKET_ERROR) {
		printf("Error : GUI socket bind failed \n");
		return -1;
	}
	if (listen(sockfd_gui, SOMAXCONN) == SOCKET_ERROR) {
		printf("Error : GUI socket listen failed \n");
		return -1;
	}

	sockfd_gui_client = accept(sockfd_gui, NULL, NULL);

	ZeroMemory(buffer, sizeof(buffer));

	sprintf(buffer, "test");

	send(sockfd_gui_client, buffer, sizeof(buffer), 0);

	closesocket(sockfd_gui);
	closesocket(sockfd_gui_client);
}