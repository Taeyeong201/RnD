
#if defined(WIN32) || defined(_WIN32) 
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <iostream>
#include <functional>
#include <string>
#include <stdlib.h>
#include <chrono>

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/DebugOutputAppender.h>

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

	static plog::DebugOutputAppender<plog::TxtFormatter> debugOutputAppender;
	plog::init(plog::verbose, &debugOutputAppender);

	if (argc != 4) {
		printf("Command parameter does not right.\n");
		printf("%s <ip> <port> <filename>", argv[0]);
		exit(1);
	}

	if (QUIC_FAILED(QuicFramework::QuicOpen())) {
		return -1;
	}
	QuicFramework quicFramework;



	quicFramework.quicSettings_;

	const uint32_t Version = 0xff00001dU; // IETF draft 29
	quicFramework.quicSettings_.SetDesiredVersionsList(&Version, 1);
	quicFramework.quicSettings_.SetIdleTimeoutMs(100000);
	quicFramework.quicSettings_.SetDisconnectTimeoutMs(10000);
	quicFramework.quicSettings_.SetPeerBidiStreamCount(5);
	//quicFramework.quicSettings_.SetDatagramReceiveEnabled(true);
	quicFramework.quicSettings_.KeepAliveIntervalMs = 5000;
	quicFramework.quicSettings_.IsSet.KeepAliveIntervalMs = TRUE;

	//printf("%d\n", strlen("111"));

	quicFramework.initializeConfig();

	FILE* fp;
	errno_t err = fopen_s(&fp, argv[3], "rb");
	if (err != 0) {
		printf("File Do not created!");
		exit(1);
	}
	quicFramework.connection(argv[1], atoi(argv[2]));

	//std::string input_string;

	u64 start, end;
	u64 start1, end1;
	u64 testset = 0;

	char buf[BUF_SIZE];


	u64 totalBufferNum;
	u64 BufferNum;
	u64 sendBytes;
	u64 file_size;  // total file size
	u64 totalSendBytes;  // received file size

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	totalBufferNum = file_size / sizeof(buf) + 1;
	fseek(fp, 0, SEEK_SET);
	BufferNum = 0;
	totalSendBytes = 0;

	
	Sleep(1000);
	std::chrono::system_clock::time_point chronostart = std::chrono::system_clock::now();

	_snprintf_s(buf, sizeof(buf), "%llu", file_size);
	quicFramework.stream_.Send(buf, BUF_SIZE);
	printf("send first data : %llu\n", file_size);

	start = GetMicroCounter();

	while ((sendBytes = (int)fread(buf, sizeof(char), sizeof(buf), fp)) > 0)
	{
		start1 = GetMicroCounter();
		quicFramework.stream_.Send(buf, sendBytes);
		end1 = GetMicroCounter();
		testset += end1 - start1;

		BufferNum++;
		totalSendBytes += sendBytes;
		/*printf("In progress: %10d/%10d(Bytes) [%03d%%]\r", 
			totalSendBytes, 
			file_size, 
			((BufferNum * 100) / totalBufferNum));*/
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
	//printf("\nElapsed Time (micro seconds) : %lld\n", end - start);
	//printf("Elapsed Time (micro seconds) : %lld\n", end - start);

	getchar();

	quicFramework.close();

	QuicFramework::QuicClose();

	printf("end");

	return 0;
}