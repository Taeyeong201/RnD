
#if defined(WIN32) || defined(_WIN32) 
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <iostream>
#include <functional>
#include <string>
#include <stdlib.h>

#include "QUIC_Framework.h"
typedef unsigned long long u64;

#define BUF_SIZE 1024

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

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("Command parameter does not right.\n");
		printf("%s <port> <filename>", argv[0]);
		exit(1);
	}

	if (QUIC_FAILED(QuicFramework::QuicOpen())) {
		return -1;
	}
	QuicFramework quicFramework;



	quicFramework.quicSettings_;
	quicFramework.quicSettings_.SetIdleTimeoutMs(50000);
	quicFramework.quicSettings_.SetServerResumptionLevel(QUIC_SERVER_RESUME_AND_ZERORTT);
	quicFramework.quicSettings_.SetDisconnectTimeoutMs(10000);
	quicFramework.quicSettings_.SetPeerBidiStreamCount(5);
	//quicFramework.quicSettings_.SetDatagramReceiveEnabled(true);
	quicFramework.quicSettings_.SetMinimumMtu(3000);
	quicFramework.quicSettings_.SetMaximumMtu(4000);

	//printf("%d\n", strlen("111"));

	quicFramework.initializeConfig();

	FILE* fp;
	errno_t err = fopen_s(&fp, argv[2], "rb");
	if (err != 0) {
		printf("File Do not created!");
		exit(1);
	}
	quicFramework.connection("127.0.0.1", atoi(argv[1]));

	//std::string input_string;

	u64 start, end;

	char buf[BUF_SIZE];


	int totalBufferNum;
	int BufferNum;
	int sendBytes;
	int file_size;  // total file size
	int totalSendBytes;  // received file size

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	totalBufferNum = file_size / sizeof(buf) + 1;
	fseek(fp, 0, SEEK_SET);
	BufferNum = 0;
	totalSendBytes = 0;

	getchar();

	_snprintf_s(buf, sizeof(buf), "%d", file_size);
	quicFramework.stream_.Send(buf, BUF_SIZE);
	printf("send first data : %d\n", file_size);

	start = GetMicroCounter();

	while ((sendBytes = (int)fread(buf, sizeof(char), sizeof(buf), fp)) > 0)
	{
		quicFramework.stream_.Send(buf, sendBytes);

		BufferNum++;
		totalSendBytes += sendBytes;
		/*printf("In progress: %10d/%10d(Bytes) [%03d%%]\r", 
			totalSendBytes, 
			file_size, 
			((BufferNum * 100) / totalBufferNum));*/
	}

	end = GetMicroCounter();

	printf("\nElapsed Time (micro seconds) : %lld\n", end - start);

	getchar();

	quicFramework.close();

	QuicFramework::QuicClose();


	return 0;
}