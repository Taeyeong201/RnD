
#include <iostream>
#include <future>
#include <chrono>
#include <memory>
#include <thread>
#include <Windows.h>

using namespace std;

int main() {
	unique_ptr<std::promise<bool>> infoPromise;
	unique_ptr<std::promise<bool>> infoPromise1;
	unique_ptr<std::future<bool>> infoFuture;
	unique_ptr<std::future<bool>> infoFuture1;
	infoPromise = make_unique<std::promise<bool>>();
	//infoFuture = make_unique<std::future<bool>>();
	infoFuture.reset(&infoPromise->get_future());
	thread t = thread([&] {
		Sleep(1000);
		infoFuture.reset(&infoPromise1->get_future());
	});
	//infoFuture1.reset(&infoPromise->get_future());

	//infoFuture->wait();
	Sleep(5000);

	return 0;
}
