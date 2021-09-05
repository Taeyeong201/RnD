//https://github.com/progschj/ThreadPool

#include "ThreadPool.h"
using namespace ThreadPool;

// 사용 예시
int work(int t, int id) {
	printf("Thread ID(%d) Start !\n", id);
	std::this_thread::sleep_for(std::chrono::seconds(t));
	printf("Thread ID(%d) End after %ds\n", id, t);
	return t + id;
}

int main() {
	ThreadPool pool(3);

	std::vector<std::future<int>> futures;

	for (int i = 0; i < 10; i++) {
		futures.emplace_back(pool.EnqueueJob(work, i % 3 + 1, i));
	}
	for (auto& f : futures) {
		printf("Result : %d \n", f.get());
	}
}