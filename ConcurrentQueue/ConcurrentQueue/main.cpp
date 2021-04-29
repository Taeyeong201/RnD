#include "../concurrentqueue.h"
#include "../blockingconcurrentqueue.h"

#include <iostream>

using namespace moodycamel;


void bulk() {
	ConcurrentQueue<int> q;
	int dequeued[100] = { 0 };
	std::thread threads[20];

	// Producers
	for (int i = 0; i != 10; ++i) {
		threads[i] = std::thread([&](int i) {
			int items[10];
			for (int j = 0; j != 10; ++j) {
				items[j] = i * 10 + j;
			}
			q.enqueue_bulk(items, 10);
		}, i);
	}

	// Consumers
	for (int i = 10; i != 20; ++i) {
		threads[i] = std::thread([&]() {
			int items[20];
			for (std::size_t count = q.try_dequeue_bulk(items, 20); count != 0; --count) {
				++dequeued[items[count - 1]];
			}
		});
	}

	// Wait for all threads
	for (int i = 0; i != 20; ++i) {
		threads[i].join();
	}

	// Collect any leftovers (could be some if e.g. consumers finish before producers)
	int items[10];
	std::size_t count;
	while ((count = q.try_dequeue_bulk(items, 10)) != 0) {
		for (std::size_t i = 0; i != count; ++i) {
			++dequeued[items[i]];
		}
	}

	// Make sure everything went in and came back out!
	for (int i = 0; i != 100; ++i) {
		assert(dequeued[i] == 1);
	}
}

class Item {
public:
	Item(int _i) : i(_i) {

	}
	Item() : i(0) {

	}

	static Item produceItem() {
		return Item(1);
	}
	static void consumeItem(Item& item) {
		if (item.i != 1) {
			std::cout << "broken Item" << std::endl;
		}
	}

private:
	int i;
};

void blockingTest() {
	BlockingConcurrentQueue<Item> q;
	const int ProducerCount = 8;
	const int ConsumerCount = 8;
	std::thread producers[ProducerCount];
	std::thread consumers[ConsumerCount];
	std::atomic<int> promisedElementsRemaining(ProducerCount * 1000);
	for (int i = 0; i != ProducerCount; ++i) {
		producers[i] = std::thread([&]() {
			for (int j = 0; j != 1000; ++j) {
				q.enqueue(Item::produceItem());
			}
		});
	}
	for (int i = 0; i != ConsumerCount; ++i) {
		consumers[i] = std::thread([&]() {
			Item item;
			while (promisedElementsRemaining.fetch_sub(1, std::memory_order_relaxed)) {
				q.wait_dequeue(item);
				Item::consumeItem(item);
			}
		});
	}
	for (int i = 0; i != ProducerCount; ++i) {
		producers[i].join();
	}

	for (int i = 0; i != ConsumerCount; ++i) {
		consumers[i].join();
	}
}

int main() {
	//bulk();
	blockingTest();

	return 0;
}