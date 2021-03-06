#include <iostream>
#include <memory>

#include "blockingconcurrentqueue.h"
#include "concurrentqueue.h"

using namespace moodycamel;

template <typename T>
struct Array_Deleter
{
	void operator()(T* p)
	{
		delete[] p;
	}
};

//class Packet {
//public:
//	Packet() :data(nullptr)
//	{
//
//	}
//	Packet(Packet&& pack) {
//		this->data = std::move(pack.data);
//		std::cout << "move" << std::endl;
//	}
//
//	std::shared_ptr<int> data;
//};

struct Packet {
	std::shared_ptr<uint8_t> data;
};

BlockingConcurrentQueue<std::shared_ptr<int>> q;
BlockingConcurrentQueue<Packet> q_1;


std::shared_ptr<int> test(std::shared_ptr<int>&& data) {
	std::shared_ptr<int> test;
	test.swap(data);
	if (!test) {
		std::cout << "test null" << std::endl;
	}
	return std::move(test);
}

std::shared_ptr<int> test2() {
	std::shared_ptr<int> test;
	q.try_dequeue(test);
	if (!test) {
		std::cout << "test null" << std::endl;
	}
	return std::move(test);
}

void test3(std::shared_ptr<int>& data) {
	q.try_dequeue(data);
	if (!data) {
		std::cout << "test null" << std::endl;
	}
	return;
}
void test4(std::shared_ptr<uint8_t>& data) {
	Packet test;
	q_1.try_dequeue(test);
	data = std::move(test.data);
	//return std::move(test);
}

/*
	공유 포인터는 스마트 포인터의 한 종류로
	유니크 포인터와 달리 소유권을 다른 공유 포인터와 공유할 수 있으며,
	참조 카운팅 기반으로 동작하는 스마트 포인터이다.
	공유 포인터는 멤버 변수로 원시 포인터를 가리키는 포인터 1개와 참조 카운팅 관리를 위한 포인터 1개를 가진다.
*/

int main() {
	auto imgbuffer(std::make_shared<unsigned char>(4096));
	std::shared_ptr<int> s1 = std::make_shared<int>(32);
	std::shared_ptr<int> s2 = std::make_shared<int>(32);
	std::shared_ptr<int> s3;
	//배열할당하기
	std::shared_ptr<int> s2 = std::make_shared<int>(32)
	/*
		내부적으로 역참조 연산자  * 가 오버로딩 되어 있으며,
		멤버에 접근하기 위한  ->  연산자 또한 오버로딩되어 있다.
		shared_ptr 자체의 멤버 함수를 호출하기 위해선 평소처럼  . 를 사용한다.
	*/
	//s1->func() 
	q.enqueue(std::move(s1));

	test3(s3);
	std::cout << "s1 use count " << s1.use_count() << std::endl;
	std::cout << "s2 use count " << s2.use_count() << std::endl;
	std::cout << "s3 use count " << s3.use_count() << std::endl;
	if (!s1) {
		std::cout << "s1 null" << std::endl;
	}

	Packet packet;
	packet.data = std::make_shared<uint8_t>(4096);

	q_1.enqueue(std::move(packet));
	std::shared_ptr<uint8_t> s4;

	test4(s4);
	std::cout << "s4 use count " << s4.use_count() << std::endl;
	std::cout << "packet.data use count " << packet.data.use_count() << std::endl;

	/*
		[원시 포인터 참조 끊기, reset 함수1]
			reset 멤버 함수를 통해 공유 포인터가 참조하고 있는 원시 포인터를 지울 수 있다.
			여기서 지운다는 것은 delete가 아니라 단순히 더 이상 참조하지 않겠다는 뜻이다.
			따라서 참조 카운트가 1 감소한다.
			만약 여전히 해당 원시 포인터를 누군가 참조하고 있다면 원시 포인터는 살아있을 것이고,
			그렇지 않다면 delete를 통해 소멸시킬 것이다. 즉 참조 카운트를 감소시키고 0이라면 지운다.
			reset 대신 공유 포인터에 nullptr을 대입해도 동일한 기능을 수행한다.

		[원시 포인터 참조 끊기, reset 함수2]
			오버로딩된 reset 함수를 통해 다른 원시 포인터로 재설정할 수 있다.
			인자로 원시 포인터를 받는다.
			이 또한 이전 원시 포인터에 대한 참조 카운트가 0이 된다면 메모리를 해제하고 새 원시 포인터로 설정한다.

		[참조 카운팅 방식의 문제점]
			shared_ptr가 사용하는 참조 카운팅 방식에 문제점이 하나 있다.
			바로 순환 참조(circular dependency)라는 것이다.

			순환 참조란 참조 횟수를 가지는 두 개체가 서로를 참조하고 있어
			참조 카운트가 0이 되지 못해 스스로 메모리 해제가 불가능한 상황을 말한다.


	[멀티 스레드 환경에서의 문제점]
		이는 참조 카운트 방식에서의 문제점이지만 shared_ptr에선 해결이 된다.
		참조 카운트는 메모리가 참조될 때마다 참조 카운트를 증가시키고, 참조되지 않을 때마다 감소시킨다.
		이는 멀티 스레드 환경에서 문제가 될 수 있다.
		예를 들어 현재 참조 카운트가 1인 메모리 M을 스레드 A가 참조하려고 참조 카운트를 1 증가시키려고 한다.
		그런데 스레드 B 또한 M을 참조하기 위해 참조 카운트를 1 증가시키려고 한다.
		그런데 스레드 A, B 모두 M의 참조 카운트가 1일 때 가져가버렸다.
		정상적인 경우라면 카운트는 2 증가하여야 하는데, 두 스레드 모두 1에서 증가시켜 2가 되었다.

		이는 멀티 스레드 환경에서 발생할 수 있는 race condtion(경쟁 상태)라는 상태이며
		lock 또는 mutex와 같은 함수를 통해 두개 이상의 스레드가 동시에 접근하지 못하도록 막아야 하는데,
		이 경우 속도가 느려지게 된다. shared_ptr는 내부적으로 이 문제점을 해결하여 등장하였으며 최적화가 되어 있다.
		멀티 스레드 환경에서 shared_ptr를 사용하지 않을 경우,
		참조 카운팅 방식은 기존의 C스타일 포인터보다 느리다.
		왜냐면 참조 카운트라는 공유 자원을 사용하기 위해선 lock과 같은 함수가 필요하기 때문이다.
	*/
	return 0;
}