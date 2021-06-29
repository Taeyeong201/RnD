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
	QuicFramework quicFramework("server");


	quicFramework.quicSettings_;
	quicFramework.quicSettings_.SetIdleTimeoutMs(50000);
	quicFramework.quicSettings_.SetDisconnectTimeoutMs(10000);
	quicFramework.quicSettings_.SetPeerBidiStreamCount(5);
	//quicFramework.quicSettings_.SetDatagramReceiveEnabled(true);
	quicFramework.quicSettings_.SetMinimumMtu(3000);
	quicFramework.quicSettings_.SetMaximumMtu(4000);

	quicFramework.initializeConfig();

	FILE* fp;
	errno_t err = fopen_s(&fp, argv[2], "wb");
	if (err != 0) {
		printf("File Do not created!");
		exit(1);
	}
	quicFramework.startListener(atoi(argv[1]));
	quicFramework.stream_.InitializeReceive();

	DataPacket data = { 0, };

	u64 start, end;
	u64 start1, end1;
	u64 start2, end2;
	u64 testset = 0;
	u64 testset1 = 0;
	int totalBufferNum;
	int BufferNum;
	long file_size;
	long totalReadBytes;

	char buf[BUF_SIZE] = { 0, };

	quicFramework.stream_.receiveData(data);
	memcpy(buf, data.data.get(), data.size);
	file_size = atol(buf);
	printf("file size : %d\n", file_size);
	totalBufferNum = file_size / BUF_SIZE + 1;
	BufferNum = 0;
	totalReadBytes = 0;

	start = GetMicroCounter();

	while (BufferNum != totalBufferNum)
	{
		start1 = GetMicroCounter();
		quicFramework.stream_.receiveData(data);
		end1 = GetMicroCounter();
		testset += end1 - start1;

		BufferNum++;
		totalReadBytes += data.size;
		//printf("In progress: %10d/%10d(Bytes) [%03d%%]\r",
		//	totalReadBytes, file_size, ((BufferNum * 100) / totalBufferNum));
		start2 = GetMicroCounter();
		fwrite(data.data.get(), sizeof(char), data.size, fp);
		end2 = GetMicroCounter();
		testset1 += end2 - start2;
	}
	end = GetMicroCounter();
	printf("\nElapsed Time (micro seconds) : %lld\n", end - start);
	printf("\nElapsed Time (micro seconds) : %lld\n", testset);
	printf("\nElapsed Time (micro seconds) : %lld\n", testset1);

	getchar();

	fclose(fp);

	quicFramework.close();
	QuicFramework::QuicClose();

}