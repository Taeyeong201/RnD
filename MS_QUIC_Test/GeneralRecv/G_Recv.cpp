#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#if defined(_WIN32)
#define sleep(sec)  Sleep((sec)*1000)
#endif
typedef unsigned long long u64;
u64 GetMicroCounter();

void udp(int argc, char** argv);
void tcp(int argc, char** argv);

#define BUF_SIZE 1024

int main(int argc, char** argv) {
	WSADATA wsaData;
	int iResult;

	if (argc != 3) {
		printf("Command parameter does not right.\n");
		printf("%s <port> <filename>", argv[0]);
		exit(1);
	}
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		exit(1);
	}

	//udp(argc, argv);
	tcp(argc, argv);

	WSACleanup();


	return 0;
}

u64 GetMicroCounter()
{
	u64 Counter = 0;

#if defined(_WIN32)
	u64 Frequency = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);
	QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
	Counter = 1000000 * Counter / Frequency;
#elif defined(__linux__)
	struct timeval t;
	gettimeofday(&t, 0);
	Counter = 1000000 * t.tv_sec + t.tv_usec;
#endif

	return Counter;
}

void udp(int argc, char** argv) 
{
	u64 start, end;
	SOCKET s;
	struct sockaddr_in local_addr;

	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Socket Creat Error.\n");
		exit(1);
	}

	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	local_addr.sin_port = htons(atoi(argv[1]));

	if (bind(s, (SOCKADDR*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR)
		printf("BIND ERROR\n");

	printf("This server is waiting... \n");

	//struct sockaddr_in client_addr;
	struct sockaddr client_addr;
	int len_addr = sizeof(client_addr);
	int totalBufferNum;
	int BufferNum;
	int readBytes;
	long file_size;
	long totalReadBytes;

	char buf[BUF_SIZE];

	u64 start1, end1;
	u64 start2, end2;
	u64 start3, end3;
	u64 testset = 0;
	u64 testset1 = 0;
	u64 testset2 = 0;

	FILE* fp;
	errno_t err = fopen_s(&fp, argv[2], "wb");
	if (err != 0) {
		printf("File Do not created!");
		exit(1);
	}

	readBytes = recvfrom(s, buf, BUF_SIZE, 0, &client_addr, &len_addr);
	file_size = atol(buf);
	totalBufferNum = file_size / BUF_SIZE + 1;
	BufferNum = 0;
	totalReadBytes = 0;
	start = GetMicroCounter();

	while (BufferNum != totalBufferNum) {
		start1 = GetMicroCounter();
		readBytes = recvfrom(s, buf, BUF_SIZE, 0, &client_addr, &len_addr);
		end1 = GetMicroCounter();
		testset += end1 - start1;

		BufferNum++;
		totalReadBytes += readBytes;
		//printf("In progress: %10d/%10d(Bytes) [%03d%%]\r",
		//    totalReadBytes, file_size, ((BufferNum * 100) / totalBufferNum));
		if (readBytes > 0) {
			start2 = GetMicroCounter();

			fwrite(buf, sizeof(char), readBytes, fp);
			end2 = GetMicroCounter();
			testset1 += end2 - start2;
			start3 = GetMicroCounter();
			readBytes = sendto(s, buf, 10, 0, (SOCKADDR*)&client_addr, sizeof(client_addr));

			end3 = GetMicroCounter();
			testset2 += end3 - start3;
		}

		if (readBytes == SOCKET_ERROR)
		{
			printf("ERROR\n");
			break;
		}
	}
	end = GetMicroCounter();
	printf("\nElapsed Time (micro seconds) : %lld", end - start);
	printf("\nElapsed Time (micro seconds) : %lld\n", testset);
	printf("\nElapsed Time (micro seconds) : %lld\n", testset1);
	printf("\nElapsed Time (micro seconds) : %lld\n", testset2);

	closesocket(s);

	fclose(fp);
}

void tcp(int argc, char** argv)
{
	u64 start, end;
	SOCKET s;
	struct sockaddr_in local_addr;

	if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("Socket Creat Error.\n");
		exit(1);
	}

	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	local_addr.sin_port = htons(atoi(argv[1]));

	if (bind(s, (SOCKADDR*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR)
		printf("BIND ERROR\n");

	printf("This server is waiting... \n");

	if (::listen(s, 5) == SOCKET_ERROR)
	{
		printf("Listen ERROR\n");

	}
	int addrlen = sizeof local_addr;

	SOCKET clientfd = ::accept(s, (struct sockaddr*)&local_addr, &addrlen);

	int totalBufferNum;
	int BufferNum;
	int readBytes;
	long file_size;
	long totalReadBytes;

	char buf[BUF_SIZE];

	u64 start1, end1;
	u64 start2, end2;
	u64 start3, end3;
	u64 testset = 0;
	u64 testset1 = 0;
	u64 testset2 = 0;

	FILE* fp;
	errno_t err = fopen_s(&fp, argv[2], "wb");
	if (err != 0) {
		printf("File Do not created!");
		exit(1);
	}

	readBytes = recv(clientfd, buf, BUF_SIZE, 0);
	file_size = atol(buf);
	totalBufferNum = file_size / BUF_SIZE + 1;
	BufferNum = 0;
	totalReadBytes = 0;
	start = GetMicroCounter();

	while (BufferNum != totalBufferNum) {
		start1 = GetMicroCounter();
		readBytes = recv(clientfd, buf, BUF_SIZE, 0);
		end1 = GetMicroCounter();
		testset += end1 - start1;

		BufferNum++;
		totalReadBytes += readBytes;
		//printf("In progress: %10d/%10d(Bytes) [%03d%%]\r",
		//    totalReadBytes, file_size, ((BufferNum * 100) / totalBufferNum));
		if (readBytes > 0) {
			start2 = GetMicroCounter();

			fwrite(buf, sizeof(char), readBytes, fp);
			end2 = GetMicroCounter();
			testset1 += end2 - start2;
		}

		if (readBytes == SOCKET_ERROR)
		{
			printf("ERROR\n");
			break;
		}
		}
	end = GetMicroCounter();
	printf("\nElapsed Time (micro seconds) : %lld", end - start);
	printf("\nElapsed Time (micro seconds) : %lld\n", testset);
	printf("\nElapsed Time (micro seconds) : %lld\n", testset1);
	printf("\nElapsed Time (micro seconds) : %lld\n", testset2);

	closesocket(s);

	fclose(fp);

}
