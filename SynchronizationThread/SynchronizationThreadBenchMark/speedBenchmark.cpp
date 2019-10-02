#ifdef _WIN32
#include <Windows.h>
#endif
#include <mutex>
#include <boost/thread/mutex.hpp>
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>

const int g_cRepeatCount = 1000000;
const int g_cThreadCount = 16;
LONG CritSecCount = 0;
LONG BoostMutexCount = 0;
std::atomic<LONG>  AtomicCount = 0;
LONG MutexCount = 0;
LONG InterlockedCount = 0;

double g_shmem = 8;
std::recursive_mutex g_mutex;
boost::mutex g_boostMutex;

void sharedFunc(int i)
{
	if (i % 2 == 0)
		g_shmem = sqrt(g_shmem);
	else
		g_shmem *= g_shmem;
}

#ifdef _WIN32
CRITICAL_SECTION g_critSec;
void threadFuncCritSec()
{
	for (int i = 0; i < g_cRepeatCount; ++i)
	{
		EnterCriticalSection(&g_critSec);
		//sharedFunc(i);
		CritSecCount++;
		LeaveCriticalSection(&g_critSec);
	}
}
#else
pthread_mutex_t pt_mutex;
void threadFuncPtMutex()
{
	for (int i = 0; i < g_cRepeatCount; ++i) {
		pthread_mutex_lock(&pt_mutex);
		sharedFunc(i);
		pthread_mutex_unlock(&pt_mutex);
	}
}
#endif

void threadFuncMutex()
{
	int counter = 0;
	for (int i = 0; i < g_cRepeatCount; ++i)
	{
		g_mutex.lock();
		MutexCount++;
		g_mutex.unlock();
	}
}

void threadFuncBoostMutex()
{
	for (int i = 0; i < g_cRepeatCount; ++i)
	{
		g_boostMutex.lock();
		BoostMutexCount++;
		g_boostMutex.unlock();
	}
}

void threadFuncAtomic() {
	for (int i = 0; i < g_cRepeatCount; ++i)
	{
		AtomicCount++;
	}
}
void threadFuncInterlocked() {
	for (int i = 0; i < g_cRepeatCount; ++i)
	{
		InterlockedIncrement(&InterlockedCount);
	}
}

void testRound(int threadCount)
{
	std::vector<std::thread> threads;

	std::cout << "\nThread count: " << threadCount << "\n\r";
	{
		auto startMutex = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < threadCount; ++i)
			threads.push_back(std::thread(threadFuncMutex));
		for (std::thread& thd : threads)
			thd.join();
		threads.clear();
		auto endMutex = std::chrono::high_resolution_clock::now();

		std::cout << "std::mutex:       ";
		std::cout << (double)std::chrono::duration_cast<std::chrono::microseconds>
			(endMutex - startMutex).count() / 1000;
		std::cout << "ms \n\r";
	}

	{
		auto startBoostMutex = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < threadCount; ++i)
			threads.push_back(std::thread(threadFuncBoostMutex));
		for (std::thread& thd : threads)
			thd.join();
		threads.clear();
		auto endBoostMutex = std::chrono::high_resolution_clock::now();

		std::cout << "boost::mutex:     ";
		std::cout << (double)std::chrono::duration_cast<std::chrono::microseconds>
			(endBoostMutex - startBoostMutex).count() / 1000;
		std::cout << "ms \n\r";
	}

	{
#ifdef _WIN32
		auto startCritSec = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < threadCount; ++i)
			threads.push_back(std::thread(threadFuncCritSec));
		for (std::thread& thd : threads)
			thd.join();
		threads.clear();
		auto endCritSec = std::chrono::high_resolution_clock::now();

		std::cout << "CRITICAL_SECTION: ";
		std::cout << (double)std::chrono::duration_cast<std::chrono::microseconds>
			(endCritSec - startCritSec).count() / 1000;
		std::cout << "ms \n\r";

#else
		auto startPThread = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < threadCount; ++i)
			threads.push_back(std::thread(threadFuncPtMutex));
		for (std::thread& thd : threads)
			thd.join();
		threads.clear();
		auto endPThread = std::chrono::high_resolution_clock::now();

		std::cout << "pthreads:         ";
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(endPThread - startPThread).count();
		std::cout << "us \n";
#endif
	}

	{
		auto startAtomic = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < threadCount; ++i)
			threads.push_back(std::thread(threadFuncAtomic));
		for (std::thread& thd : threads)
			thd.join();
		threads.clear();
		auto endAtomic = std::chrono::high_resolution_clock::now();

		std::cout << "Atomic:           ";
		std::cout << (double)std::chrono::duration_cast<std::chrono::microseconds>
			(endAtomic - startAtomic).count() / 1000;
		std::cout << "ms \n\r";
	}

	{
		auto startInterlocked = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < threadCount; ++i)
			threads.push_back(std::thread(threadFuncInterlocked));
		for (std::thread& thd : threads)
			thd.join();
		threads.clear();
		auto endInterlocked = std::chrono::high_resolution_clock::now();

		std::cout << "Interlocked:      ";
		std::cout << (double)std::chrono::duration_cast<std::chrono::microseconds>
			(endInterlocked - startInterlocked).count() / 1000;
		std::cout << "ms \n\r";
	}
}

int main()
{
#ifdef _WIN32
	InitializeCriticalSection(&g_critSec);
#else
	pthread_mutex_init(&pt_mutex, 0);
#endif

	std::cout << "Iterations: " << g_cRepeatCount << "\n\r";

	for (int i = 1; i <= g_cThreadCount; i = i * 2)
	{
		testRound(i);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

#ifdef _WIN32
	DeleteCriticalSection(&g_critSec);
#else
	pthread_mutex_destroy(&pt_mutex);
#endif

	return 0;
}

/*
****************************************************
****	OS		:	Windows 10 Home 64bit		****
****	VS		:	VS 141						****
****	CPU		:	Intel i3-8100				****
****	RAM		:	16GB						****
****************************************************

Iterations: 1000000

Thread count: 1
std::mutex:       22.132ms
boost::mutex:     12.246ms
CRITICAL_SECTION: 16.12ms
Atomic:           5.224ms
Interlocked:      5.518ms

Thread count: 2
std::mutex:       60.141ms
boost::mutex:     45.158ms
CRITICAL_SECTION: 69.865ms
Atomic:           33.607ms
Interlocked:      36.396ms

Thread count: 4
std::mutex:       111.146ms
boost::mutex:     69.639ms
CRITICAL_SECTION: 274.21ms
Atomic:           68.937ms
Interlocked:      71.666ms

Thread count: 8
std::mutex:       220.344ms
boost::mutex:     133.289ms
CRITICAL_SECTION: 883.093ms
Atomic:           145.078ms
Interlocked:      141.326ms

Thread count: 16
std::mutex:       440.849ms
boost::mutex:     264.066ms
CRITICAL_SECTION: 1643.39ms
Atomic:           289.888ms
Interlocked:      288.546ms

*/