#if defined(WIN32) || defined(_WIN32) 
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <iostream>
#include <functional>
#include <string>
#include <stdlib.h>
#include <chrono>

#include "QUIC_Framework.h"

typedef unsigned long long u64;

#define BUF_SIZE 8192

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
	quicFramework.quicSettings_.SetIdleTimeoutMs(100000);
	quicFramework.quicSettings_.SetDisconnectTimeoutMs(10000);
	quicFramework.quicSettings_.SetPeerBidiStreamCount(5);
	quicFramework.quicSettings_.SetServerResumptionLevel(QUIC_SERVER_RESUME_AND_ZERORTT);
	//quicFramework.quicSettings_.SetDatagramReceiveEnabled(true);

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

	u64 totalBufferNum;
	u64 BufferNum;
	u64 file_size;
	u64 totalReadBytes;

	char buf[BUF_SIZE] = { 0, };


	quicFramework.stream_.receiveData(data);
	memcpy(buf, data.data.get(), data.size);
	file_size = atol(buf);
	printf("file size : %llu\n", file_size);
	totalBufferNum = file_size / BUF_SIZE + 1;
	BufferNum = 0;
	totalReadBytes = 0;

	std::chrono::system_clock::time_point chronostart = std::chrono::system_clock::now();

	start = GetMicroCounter();
	bool error = false;
	while (BufferNum != totalBufferNum)
	{
		start1 = GetMicroCounter();
		error = quicFramework.stream_.receiveData(data);
		if (!error) {
			printf("buffer error \n");
			break;
		}
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

	std::chrono::system_clock::time_point chronoend = std::chrono::system_clock::now();

	std::chrono::milliseconds mill
		= std::chrono::duration_cast<std::chrono::milliseconds>(chronoend - chronostart);
	std::chrono::seconds sec
		= std::chrono::duration_cast<std::chrono::seconds>(chronoend - chronostart);
	std::chrono::minutes min
		= std::chrono::duration_cast<std::chrono::minutes>(chronoend - chronostart);
	std::cout << "time : " << mill.count() << " milliseconds" << std::endl;
	std::cout << "time : " << sec.count() << " seconds" << std::endl;
	std::cout << "time : " << min.count() << " minutes" << std::endl;
	//printf("\nTotal Recv : %lld\n", totalReadBytes);
	//printf("\nElapsed Time (micro seconds) : %lld\n", end - start);
	//printf("\nElapsed Time (micro seconds) : %lld\n", testset);
	//printf("\nElapsed Time (micro seconds) : %lld\n", testset1);
	//printf("\nElapsed Time (micro seconds) : %lld\n", quicFramework.stream_.test123123123());
	//printf("\nrecv total interval : %llu\n", quicFramework.stream_.testtime);
	//printf("\ncount : %lld\n", quicFramework.stream_.receiver_.t2);
	//printf("\ncount : %lld\n", quicFramework.stream_.receiver_.t3);
	//printf("\ncount : %lld\n", quicFramework.stream_.receiver_.t4);
	//printf("\ncount : %lld\n", quicFramework.stream_.receiver_.t5);

	fclose(fp);

	quicFramework.close();
	QuicFramework::QuicClose();
	getchar();

}