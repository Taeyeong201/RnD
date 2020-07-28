#include <queue>
#include <iostream>
#include <mutex>
#include <chrono>
#include <string>

template<typename ... Args>
std::string format_string(const std::string& format, Args ... args)
{
	size_t size = snprintf(nullptr, 0, format.c_str(), args ...);
	size++;
	std::unique_ptr<char[]> buffer(new char[size]);
	snprintf(buffer.get(), size, format.c_str(), args ...);
	return std::string(buffer.get(), buffer.get() + size - 1);
}

void pushThread(std::queue<std::string>* testQueue, std::mutex*m, std::condition_variable* cv) {
	std::thread::id this_id = std::this_thread::get_id();

	for (int i = 0; i < 3; i++) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::string text = format_string("thread(%d) : %d", this_id, i);
		m->lock();
		//std::cout << text << std::endl;
		testQueue->push(text);
		m->unlock();
		cv->notify_one();
	}

}
void popThread(std::queue<std::string>* testQueue, std::mutex* m, std::condition_variable* cv, bool *isRun) {
	std::thread::id this_id = std::this_thread::get_id();

	while (*isRun) {
		std::unique_lock<std::mutex> lk(*m);

		cv->wait(
			lk, [&] { return !testQueue->empty() || !*isRun; });

		if (!*isRun) {
			lk.unlock();
			return;
		}

		std::string content = testQueue->front();
		testQueue->pop();

		std::cout << content << std::endl;

		lk.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
void stopSignal(bool* isRun) {
	std::this_thread::sleep_for(std::chrono::seconds(3));
	*isRun = false;

}

int main() {
	std::queue<std::string> test;
	std::mutex m;
	std::condition_variable cv;

	bool isRun = true;

	std::vector<std::thread> pushT;
	for (int i = 0; i < 5; i++)
		pushT.push_back(std::thread(pushThread, &test, &m, &cv));

	std::thread popT = std::thread(popThread, &test, &m, &cv, &isRun);
	std::thread sig = std::thread(stopSignal, &isRun);

	for (int i = 0; i < 5; i++)
		pushT[i].join();


	sig.join();
	cv.notify_one();
	popT.join();


	return 0;
}