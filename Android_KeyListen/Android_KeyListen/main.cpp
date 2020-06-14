#/*
   *    C/C++ -> 전처리기 -> 전처리기 정의 / 편집에 상속된 값 확인
   *   구성속성 -> 일반 -> 문자집합에서 변하는 것 확인
   *   const char error 해결법 : C/C++ -> 언어 -> 준수모드 / `예 progressive` 에서 `아니오`로 변경 (문법준수 관련된 부분임)
   원본 참고 사이트 : https://docs.microsoft.com/en-us/windows/desktop/winsock/complete-server-code
   헤더 : https://docs.microsoft.com/en-us/windows/desktop/api/winsock2/
   에러코드 : https://docs.microsoft.com/en-us/windows/desktop/api/winsock/nf-winsock-shutdown
*/
#ifndef UNICODE
#define UNICODE 1
#endif

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 16
#define DEFAULT_PORT "9000"

#define __BSD_STYLE 1
#define	INVALID_KEY	0xffff


typedef struct message
{
	char option[10];
	char user[20];
	char buf[1024];
	char target[256];
	int num1;
	int num2;
}MyStruct;

typedef struct common_message
{
	unsigned char message_type;
	unsigned char padding[15];
}common_message;

typedef struct mouse_message
{
	unsigned char message_type;
	unsigned char is_pressed;
	unsigned char mouse_button_key;
	unsigned char padding;
	unsigned short mouse_x;
	unsigned short mouse_y;
	unsigned char padding2[8];
}mouse_message;

typedef struct keyboard_message
{
	unsigned char message_type;
	unsigned char is_pressed;
	unsigned char padding[2];
	unsigned int keyboard_key;
	unsigned int invalidKeyCode;
	unsigned char padding2[4];
}keyboard_messge;


int __cdecl main(void)
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

#if __BSD_STYLE
	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
#else
	/* Window Socket 참고 문서 : https://docs.microsoft.com/ko-kr/windows/desktop/api/winsock2/nf-winsock2-wsasocketw
			 SOCKET WSAAPI WSASocketW(
				int                 af,
				int                 type,
				int                 protocol,
				LPWSAPROTOCOL_INFOW lpProtocolInfo,
				GROUP               g,
				DWORD               dwFlags
			 );
	   */

	ListenSocket = WSASocket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol, NULL, 0, 0);
#endif

	int opt = 1;
	setsockopt(ListenSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	printf("listen OK\n");

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	printf("accept OK\n");

	setsockopt(ClientSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt));

	// No longer need server socket
	closesocket(ListenSocket);

	//MyStruct mystruct;
	//ZeroMemory(&mystruct, sizeof(MyStruct));


	////mystruct.user = "DongHyun";
	////mystruct.target = "Window";
	////mystruct.option = "Fast";
	//strcpy(mystruct.user, "DongHyun");
	//strcpy(mystruct.target, "Window");
	//strcpy(mystruct.option, "Fast");
	//strcpy(mystruct.buf, "This is for test struct send message");
	//mystruct.num1 = 1;
	//mystruct.num2 = 2;

	//// Send an initial buffer by struct
	//iResult = send(ClientSocket, (char*)&mystruct, sizeof(MyStruct), 0);
	//if (iResult == SOCKET_ERROR) {
	//   printf("send failed with error: %d\n", WSAGetLastError());
	//   closesocket(ClientSocket);
	//   WSACleanup();
	//   return 1;
	//}

	//printf("Bytes Sent: %ld\n", iResult);

	INPUT in;
	DEVMODE   mode;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode);

	common_message m;
	keyboard_message* msgk = (keyboard_message*)&m;
	mouse_message* msgm = (mouse_message*)&m;

	// Receive until the peer shuts down the connection
	do {

		ZeroMemory(&m, sizeof(common_message));

		iResult = recv(ClientSocket, (char*)&m, sizeof(common_message), 0);

		printf("recv size : %d\n", iResult);


		if (iResult > 0) {

			ZeroMemory(&in, sizeof(in));                  //memset for Input value

			if (m.message_type == 0)
			{
				in.type = INPUT_KEYBOARD;
				//SDLKeyToKeySym(msgk->keyboard_key)
				printf("RECV_KEY : %d\n", msgk->keyboard_key);
				if ((in.ki.wVk = msgk->keyboard_key) != INVALID_KEY) {
					if (msgk->is_pressed == 0) {
						in.ki.dwFlags |= KEYEVENTF_KEYUP;
					}
					in.ki.wScan = MapVirtualKey(in.ki.wVk, MAPVK_VK_TO_VSC);
					SendInput(1, &in, sizeof(in));
				}
				else {
					printf("INVALID_KEY! %d\n", msgk->invalidKeyCode);
				}
			}
			else if (m.message_type == 1)
			{
				// Mouse Move
				if (msgm->padding == 1) {
					if (msgm->mouse_button_key == 1 && msgm->is_pressed != 0) {
						printf("Left mouse Button Press\n");
						in.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
						SendInput(1, &in, sizeof(in));
					}
					else if (msgm->mouse_button_key == 1 && msgm->is_pressed == 0) {
						printf("Left mouse Button NOT Press\n");
						in.mi.dwFlags = MOUSEEVENTF_LEFTUP;
						SendInput(1, &in, sizeof(in));
					}
				}
				else {
					//ZeroMemory(&in, sizeof(in));                  //memset for Input value
					printf("x : %d / y : %d\n", msgm->mouse_x, msgm->mouse_y);
					//printf("msgm->is_pressed : %d\n", msgm->is_pressed);

					in.type = INPUT_MOUSE;

					in.mi.dx = (msgm->mouse_x * 65535) / (mode.dmPelsWidth - 1);
					in.mi.dy = (msgm->mouse_y * 65535) / (mode.dmPelsHeight - 1);
					in.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
					SendInput(1, &in, sizeof(in));
				}

			}
			//else if (m.message_type == 2)
			//{
			//   // Mouse Button

			//   in.type = INPUT_MOUSE;

			//   if (msgm->mouse_button_key == 1 && msgm->is_pressed != 0) {
			//      in.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			//   }
			//   else if (msgm->mouse_button_key == 1 && msgm->is_pressed == 0) {
			//      in.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			//   }
			//   //else if (msgm->mousebutton == 2 && msgm->is_pressed != 0) {
			//   //   in.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
			//   //}
			//   //else if (msgm->mousebutton == 2 && msgm->is_pressed == 0) {
			//   //   in.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
			//   //}
			//   //else if (msgm->mousebutton == 3 && msgm->is_pressed != 0) {
			//   //   in.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			//   //}
			//   //else if (msgm->mousebutton == 3 && msgm->is_pressed == 0) {
			//   //   in.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			//   //}
			//   //else if (msgm->mousebutton == 4 && msgm->is_pressed != 0) {
			//   //   // mouse wheel click
			//   //   in.mi.dwFlags = MOUSEEVENTF_WHEEL;
			//   //   in.mi.mouseData = +WHEEL_DELTA;
			//   //}
			//   //else if (msgm->mousebutton == 5 && msgm->is_pressed != 0) {
			//   //   // mouse wheel click
			//   //   in.mi.dwFlags = MOUSEEVENTF_WHEEL;
			//   //   in.mi.mouseData = -WHEEL_DELTA;
			//   //}
			//   SendInput(1, &in, sizeof(in));
			//}
			//else if (m.message_type == '2')
			//{
			//   in.type = INPUT_KEYBOARD;
			//   in.ki.wScan = 0;
			//   in.ki.time = 0;
			//   in.ki.dwExtraInfo = 0;

			//   in.ki.wVk = (DWORD)msgk->keyboard_key;
			//   in.ki.dwFlags = msgk->is_pressed ? KEYEVENTF_KEYUP : 0;

			//   SendInput(1, &in, sizeof(INPUT));
			//}



			//printf("recv data : %s", recvbuf);
			//printf("Bytes received: %d\n", iResult);

			//// Echo the buffer back to the sender
			//iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			//if (iSendResult == SOCKET_ERROR) {
			//   printf("send failed with error: %d\n", WSAGetLastError());
			//   closesocket(ClientSocket);
			//   WSACleanup();
			//   return 1;
			//}
			//printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}