//출처: https://twinw.tistory.com/156
//파라메터 :  <server IP> <port> <filename>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#define BUF_SIZE 1024
typedef unsigned long long u64;
u64 GetMicroCounter();

int main(int argc, char** argv) {
    WSADATA wsaData;
    struct sockaddr_in server_addr;
    SOCKET s;
    u64 start, end;
    int iResult;
    

    if (argc != 4) {
        printf("Command parameter does not right.\n");
        printf("%s <server IP> <port> <filename>", argv[0]);
        exit(1);
    }
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        exit(1);
    }

    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Socket Creat Error.\n");
        exit(1);
    }

    printf("File Send Start");
    u64 start1, end1;
    u64 start2, end2;
    u64 testset = 0;
    u64 testset1 = 0;
    int totalBufferNum;
    int BufferNum;
    int sendBytes;
    int file_size;  // total file size
    int totalSendBytes;  // received file size

    char buf[BUF_SIZE];

    FILE* fp;
    errno_t err = fopen_s(&fp, argv[3], "rb");
    if (err != 0) {
        printf("File not Exist");
        exit(1);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    //server_addr.sin_addr.s_addr = inet_pton(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    totalBufferNum = file_size / sizeof(buf) + 1;
    fseek(fp, 0, SEEK_SET);
    BufferNum = 0;
    totalSendBytes = 0;

    start = GetMicroCounter();

    _snprintf_s(buf, sizeof(buf), "%d", file_size);
    sendBytes = sendto(s, buf, sizeof(char) * 1024, 0, (SOCKADDR*)&server_addr, sizeof(server_addr));

    while ((sendBytes = (int)fread(buf, sizeof(char), sizeof(buf), fp)) > 0) {
        start1 = GetMicroCounter();
        sendto(s, buf, sendBytes, 0, (SOCKADDR*)&server_addr, sizeof(server_addr));
        end1 = GetMicroCounter();
        testset += end1 - start1; 
        BufferNum++;
        totalSendBytes += sendBytes;
        //printf("In progress: %10d/%10d(Bytes) [%03d%%]\r", totalSendBytes, file_size, ((BufferNum * 100) / totalBufferNum));
        start2 = GetMicroCounter();
        while (recvfrom(s, buf, BUF_SIZE, 0, NULL, NULL) != 10)
        {
            sendto(s, buf, sendBytes, 0, (SOCKADDR*)&server_addr, sizeof(server_addr));
        }
        end2 = GetMicroCounter();
        testset1 += end2 - start2;
    }
    end = GetMicroCounter();
    printf("\nElapsed Time (micro seconds) : %lld", end - start);
    printf("\nElapsed Time (micro seconds) : %lld\n", testset);
    printf("\nElapsed Time (micro seconds) : %lld\n", testset1); 
    closesocket(s);
    WSACleanup();

    fclose(fp);

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