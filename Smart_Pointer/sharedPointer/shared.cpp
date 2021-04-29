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
	std::shared_ptr<int> data;
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
void test4(std::shared_ptr<int>& data) {
	Packet test;
	q_1.try_dequeue(test);
	data = std::move(test.data);
	//return std::move(test);
}

/*
	���� �����ʹ� ����Ʈ �������� �� ������
	����ũ �����Ϳ� �޸� �������� �ٸ� ���� �����Ϳ� ������ �� ������,
	���� ī���� ������� �����ϴ� ����Ʈ �������̴�.
	���� �����ʹ� ��� ������ ���� �����͸� ����Ű�� ������ 1���� ���� ī���� ������ ���� ������ 1���� ������.
*/

int main() {
	auto imgbuffer(std::make_shared<unsigned char>(4096));
	std::shared_ptr<int> s1 = std::make_shared<int>(32);
	std::shared_ptr<int> s2 = std::make_shared<int>(32);
	std::shared_ptr<int> s3;
	/*
		���������� ������ ������  * �� �����ε� �Ǿ� ������,
		����� �����ϱ� ����  ->  ������ ���� �����ε��Ǿ� �ִ�.
		shared_ptr ��ü�� ��� �Լ��� ȣ���ϱ� ���ؼ� ���ó��  . �� ����Ѵ�.
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
	packet.data = std::make_shared<int>(32);

	q_1.enqueue(std::move(packet));
	std::shared_ptr<int> s4;

	test4(s4);
	std::cout << "s4 use count " << s4.use_count() << std::endl;
	std::cout << "packet.data use count " << packet.data.use_count() << std::endl;

	/*
		[���� ������ ���� ����, reset �Լ�1]
			reset ��� �Լ��� ���� ���� �����Ͱ� �����ϰ� �ִ� ���� �����͸� ���� �� �ִ�.
			���⼭ ����ٴ� ���� delete�� �ƴ϶� �ܼ��� �� �̻� �������� �ʰڴٴ� ���̴�.
			���� ���� ī��Ʈ�� 1 �����Ѵ�.
			���� ������ �ش� ���� �����͸� ������ �����ϰ� �ִٸ� ���� �����ʹ� ������� ���̰�,
			�׷��� �ʴٸ� delete�� ���� �Ҹ��ų ���̴�. �� ���� ī��Ʈ�� ���ҽ�Ű�� 0�̶�� �����.
			reset ��� ���� �����Ϳ� nullptr�� �����ص� ������ ����� �����Ѵ�.

		[���� ������ ���� ����, reset �Լ�2]
			�����ε��� reset �Լ��� ���� �ٸ� ���� �����ͷ� �缳���� �� �ִ�.
			���ڷ� ���� �����͸� �޴´�.
			�� ���� ���� ���� �����Ϳ� ���� ���� ī��Ʈ�� 0�� �ȴٸ� �޸𸮸� �����ϰ� �� ���� �����ͷ� �����Ѵ�.

		[���� ī���� ����� ������]
			shared_ptr�� ����ϴ� ���� ī���� ��Ŀ� �������� �ϳ� �ִ�.
			�ٷ� ��ȯ ����(circular dependency)��� ���̴�.

			��ȯ ������ ���� Ƚ���� ������ �� ��ü�� ���θ� �����ϰ� �־�
			���� ī��Ʈ�� 0�� ���� ���� ������ �޸� ������ �Ұ����� ��Ȳ�� ���Ѵ�.


	[��Ƽ ������ ȯ�濡���� ������]
		�̴� ���� ī��Ʈ ��Ŀ����� ������������ shared_ptr���� �ذ��� �ȴ�.
		���� ī��Ʈ�� �޸𸮰� ������ ������ ���� ī��Ʈ�� ������Ű��, �������� ���� ������ ���ҽ�Ų��.
		�̴� ��Ƽ ������ ȯ�濡�� ������ �� �� �ִ�.
		���� ��� ���� ���� ī��Ʈ�� 1�� �޸� M�� ������ A�� �����Ϸ��� ���� ī��Ʈ�� 1 ������Ű���� �Ѵ�.
		�׷��� ������ B ���� M�� �����ϱ� ���� ���� ī��Ʈ�� 1 ������Ű���� �Ѵ�.
		�׷��� ������ A, B ��� M�� ���� ī��Ʈ�� 1�� �� ���������ȴ�.
		�������� ����� ī��Ʈ�� 2 �����Ͽ��� �ϴµ�, �� ������ ��� 1���� �������� 2�� �Ǿ���.

		�̴� ��Ƽ ������ ȯ�濡�� �߻��� �� �ִ� race condtion(���� ����)��� �����̸�
		lock �Ǵ� mutex�� ���� �Լ��� ���� �ΰ� �̻��� �����尡 ���ÿ� �������� ���ϵ��� ���ƾ� �ϴµ�,
		�� ��� �ӵ��� �������� �ȴ�. shared_ptr�� ���������� �� �������� �ذ��Ͽ� �����Ͽ����� ����ȭ�� �Ǿ� �ִ�.
		��Ƽ ������ ȯ�濡�� shared_ptr�� ������� ���� ���,
		���� ī���� ����� ������ C��Ÿ�� �����ͺ��� ������.
		�ֳĸ� ���� ī��Ʈ��� ���� �ڿ��� ����ϱ� ���ؼ� lock�� ���� �Լ��� �ʿ��ϱ� �����̴�.
	*/
	return 0;
}