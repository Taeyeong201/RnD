#include <boost/thread/mutex.hpp>
#include <boost/thread/scoped_thread.hpp>
#include <boost/asio.hpp>
#include <boost/random.hpp>
#include <chrono>
#include <iostream>

boost::asio::io_context ioc;
boost::mutex mutexCap;
boost::mutex mutexEncode;

boost::random::mt19937 rng;
boost::random::uniform_int_distribution<> randomGen(10, 100);

void cap(int threadNum) {
	int wait = randomGen(rng);
	mutexCap.lock();
	std::cout <<  "cap num > " << threadNum << "\twait for :" << wait << std::endl;
	Sleep(wait);
	mutexCap.unlock();
}
void cvt(int threadNum) {
	std::cout << "cvt num > " << threadNum << std::endl;
}
void encode(int threadNum) {
	int wait = randomGen(rng);
	mutexEncode.lock();
	std::cout <<  "encode num > " << threadNum << "\twait for :" << wait << std::endl;
	Sleep(wait);
	mutexEncode.unlock();
}
void send(int threadNum) {
	std::cout << "send num > " << threadNum << std::endl;
}
void threadFunc(int threadNum) {
	for (int x = 0; x < 10; x++)
	{
		cap(threadNum);
		cvt(threadNum);
		encode(threadNum);
		send(threadNum);
	}
}
boost::mutex io_mutex;

void
runningThreadFunc() {
	std::vector<std::thread> threads;

	for (int i = 0; i < 2; i++) {
		threads.push_back(std::thread(threadFunc, i));
	}
	for (std::thread& thd : threads) thd.join();
	threads.clear();

}

//void count(int id)
//{
//	for (int i = 0; i < 10; i++)
//	{
//		boost::mutex::scoped_lock lock(io_mutex);
//		std::cout << id << ":" << i << std::endl;
//	}
//}
//
//void test_thread()
//{
//	boost::thread thrd1(boost::bind(&count, 1));
//	boost::thread thrd2(boost::bind(&count, 2));
//	thrd1.join();
//	thrd2.join();
//}

int main() {
	runningThreadFunc();
}