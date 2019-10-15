//std::promise �� std::future
//-------------------------------------------------------------------------------------------------------------------------
//<future>�� ��Ƽ������ ������ ���� �����̶�⺸�ٴ� �񵿱��� ó���� �����ϰ� ó���ϵ��� �ϴµ� ��ǥ�� �ΰ� �ִ�
//�� �� ������� ��Ƽ������ ������ �����ϴ� �Ϸ��� Ŭ������ �����Ѵ�.
//future�� �Һ���(������)�� ���� ���� ���� �����(�̷��� ���⹰)�̰�, promise�� ������(������)�� ����ϴ� �����̴�.

//���α׷��� ������, �� ������ �ƴ϶� ���� ������ �������� ���ÿ� ����Ǵ� ���� �񵿱���(asynchronous) ���� �̶�� �θ���.
//�ᱹ �񵿱��� �������� �ϰ� ���� ����, ��� �����͸� �ٸ� �����带 ���� ó���ؼ� �޾Ƴ��� ���̴�.
//���� � ������ T �� ����ؼ�, �񵿱������� ���� �޾Ƴ��ڴ� ��� �ǹ̴�, 
//�̷���(future) ������ T �� ���ϴ� �����͸� ���� �ְڴ� ��� ���(promise) �̶�� �� �� �ִ�.

/* 
��� ��ü�� �½�ũ ����� �ڷ� ä�ο� �о� �ְ�, �̷� ��ü�� �۾��� ������ ��ٷȴٰ� �ڷ� ä�ο��� ����� ������.
+-----------------+                 +----------+                 +-----------------+
| promise(�۽���) | ===== set ====> |   ä��   | ===== get ====> | future(������)  |
+-----------------+                 +----------+                 +-----------------+
*/

#include <chrono>
#include <future>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <exception>
#include <vector>

//Step 0
int asyncFunc(int value) {
	std::cerr << "Async Thread = > " << std::this_thread::get_id() << std::endl;
	std::cerr << "I am inside a async..." << std::endl;

	return(value + 100);
}
//-----------------------------------------------------

// Step1
using std::string;

void worker(std::promise<string>* p) {
	// ����� �����ϴ� ���. �ش� ����� future �� ����.
	// promise ��ü�� �ڽ��� future ��ü�� �����͸� ������ �Ŀ� wait �ؾ� �Ѵ�.
	p->set_value("some data");
}
//------------------------------------------------------

// Step 2
//������-�Һ��� ���Ͽ��� promise �� ��ġ ������(producer)�� ������ �����ϰ�, future �� �Һ���(consumer)�� ������ �����Ѵ�. 
//�׷���, promise �� future �� �̿��ϴ� ���� �ξ� �� ����ϰ� �� �����ϱ⵵ ����. 
//���� ���� ������ ����� �� ���� �� ����� ���� future �� ���ܵ� ������ �� �ֱ� �����̴�.

std::condition_variable cv;
std::mutex m;
bool done = false;
std::string info;

void worker2() { //���� ���� ���
	{
		std::lock_guard<std::mutex> lk(m);
		info = "some data";  // ���� p->set_value("some data") �� ����
		done = true;
	}
	cv.notify_all();
}
//------------------------------------------------------

// Step3 future & promise�� ���� ���� ���
void worker3(std::promise<string>* p) {
	try {
		throw std::runtime_error("Some Error!");
	}
	catch (...) {
		//set_exception ���� exception_ptr �� �����ؾ� �Ѵ�. exception_ptr �� catch �� ���� ���� ��ü�� �����Ͱ� �ƴ϶�, 
		//���� catch �� ���ܿ� ���� ������ ��ȯ�ϴ� current_exception �Լ��� �����ϴ� ��ü�̱� �����̴�.
		//����, catch �� ���޹��� ���� ��ü�� make_exception_ptr �Լ��� ����ؼ� exception_ptr �� ���� �� �� ������, 
		//�׳� ���ϰ� current_exception �� ȣ���ϴ� ���� �� �����ϴ�.
		p->set_exception(std::current_exception());
	}
}
//------------------------------------------------------

// Step4 future & promise�� wait_for ����
void worker4(std::promise<void>* p) {
	std::this_thread::sleep_for(std::chrono::seconds(10));
	p->set_value();
}
//------------------------------------------------------

// Step5 shared_future
//future �� ��� get �� ȣ���ϸ� future ������ ��ü�� �̵��Ǳ� ������ get �� �� �� ���� �� �� �ִ�
//������, ���� ���� ���� �ٸ� �����忡�� future �� get �� �ʿ伺�� �ִ�. �� �� shared_future �� ����ϸ� �ȴ�.
//�Ʒ� ������ �޸��⸦ �ϴ� ���� C++ ���α׷����� ��Ÿ���� ���̴�. 
//main �Լ����� ��� ��ȣ�� ������ �� runner ������鿡�� �޸��⸦ �����ϰ� �ȴ�.
void runner(std::shared_future<void>* start) {
	start->get();
	std::cout << "���!" << std::endl;
}
//------------------------------------------------------

// Step6 std::async �񵿱� ó��
//promise �� packaged_task �� �񵿱������� ������ �ϱ� ���ؼ���, �����带 ��������� �����ؼ� �����ؾ߸� �ߴ�. 
//������ std::async �� � �Լ��� �����Ѵٸ�, �ƿ� �����带 �˾Ƽ� ���� �ش� �Լ��� �񵿱������� �����ϰ�, �� ������� future �� �����Ѵ�.
// std::accumulate �� ����

int sum(const std::vector<int>& v, int start, int end) {
	int total = 0;
	for (int i = start; i < end; ++i) { total += v[i]; }
	return total;
}

/*async �Լ��� ���ڷ� ���� �Լ��� �񵿱������� ������ �Ŀ�, �ش� ������� ������ future �� �����Ѵ�.
ù ��° ���ڷδ� ��� ���·� ���������� �����ϴµ� �� ���� ���� �����ϴ�.
1. std::launch::async : �ٷ� �����带 �����ؼ� ���ڷ� ���޵� �Լ��� �����Ѵ�. ��, �ٷ� �� �ڸ����� �����带 �����ؼ� �����ϰ� �ȴ�.
2. std::launch::deferred : future �� get �Լ��� ȣ��Ǿ��� �� �����带 �����ؼ� �����ϱ� ������ �񵿱������� �����ϰ� �ȴ�.
   �ٽ� ����, �ش� �Լ��� ���� �ٷ� ���� �񵿱������� ������ �ʿ䰡 ���ٸ� deferred �ɼ��� �ָ� �ȴ�. */

/*���� parallel �Լ��� 1 ���� 1000 ������ ������ �� 2 ���� �����忡�� �����Ѵ�
1 ���� 500 ������ ����, async �� ���� ������ ���ο� �����忡�� ���ϰ� �ǰ�, 
������ 501 ���� 1000 ������ ���� ������ �����忡�� ó���ϰ� �ȴ�. */
int parallel_sum(const std::vector<int>& v) {
	// lower_half_future �� 1 ~ 500 ���� �񵿱������� ����
	// ����� ���� �Լ��� ����ϸ� �� �� ����ϰ� ǥ���� �� �� �ִ�.
	// --> std::async([&v]() { return sum(v, 0, v.size() / 2); });
	std::future<int> lower_half_future = std::async(std::launch::async, sum, cref(v), 0, v.size() / 2);
	//std::future<int> lower_half_future = std::async([&v]() { return sum(v, 0, v.size() / 2); });

	// upper_half �� 501 ���� 1000 ���� ����
	int upper_half = sum(v, v.size() / 2, v.size());

	//async �Լ��� �����ϴ� �Լ��� ������� �����ϴ� future �� �����Ѵ�. 
	//�׸��� async �Լ��� ������ future �� get �� ���� �� �� �ִ�.
	return lower_half_future.get() + upper_half;
}

int main() {
	//promise ��ü�� ������ ��, ������ ���� �Ŀ� ������ ��ü�� Ÿ���� ���ø� ���ڷ� �޴´�.
	std::promise<string> p; //���ڿ��� ���� ���̱� ������ string ����

	// �̷��� string �����͸� ���� �ְڴٴ� ���. get_future �Լ��� ���ؼ� ���� �� �ִ�.
	//������ ���� ������ promise ��ü�� �ڽ��� ������ �ִ� future ��ü�� ���� �־��ְ� �ȴ�. 
	//������ data �� ������ ���� ���� ����� �����ϰ� �ִ� ���� �ƴϴ�. 
	//�����忡�� data �� ���� ����� �����ϱ� ���ؼ��� "p->set_value("some data")"�� ���� ó���ؾ� �Ѵ�.
	std::future<string> data = p.get_future();

	// Step 0 ----------------------
	std::cout << "Main Thread => " << std::this_thread::get_id() << std::endl;
	std::future<int> fn = std::async(std::launch::async, asyncFunc, 100);
	if (fn.valid()) std::cout << fn.get() << std::endl;
	if (fn.valid()) fn.get();
	else std::cout << "Invalid ayncFunc" << std::endl;
	// Step 0 ----------------------

	std::thread tr1(worker, &p);	

	//�� ���� �߿��� ���� promise �� future �� ���� �����ϱ� �� ���� wait �Լ��� ��ٸ��ٴ� ���̴�.
	//wait �Լ��� ������ �Ͽ��ٸ� get �� ���ؼ� future �� ���޵� ��ü�� ���� �� �ִ�.

	data.wait(); // �̷��� ��ӵ� �����͸� ���� �� ���� ��ٸ���.
	// ����, wait�� �����ߴٴ� ���� future�� �����Ͱ� �غ�Ǿ��ٴ� �ǹ�.
	
	//����� ���� wait �Լ��� ������ ȣ���� �ʿ���� get �Լ��� �ٷ� ȣ���ϴ��� 
	//�˾Ƽ� promise �� future �� ��ü�� ������ �� ���� ��ٸ� ������ �����Ѵ�.
	//��, get �� ȣ���ϸ� future ���� �ִ� �����Ͱ� �̵� �ȴ�. ���� get �� ����� �ٽ� ȣ���ϸ� �ȵȴ�.
	
	std::cout << "Step1 ���� ������ : " << data.get() << std::endl; // �׳� get �ص� wait �� �Ͱ� �����ϴ�.
	tr1.join();
	
	//Step 2---------------------------------------------------------------------------------------------------
	std::thread tr2(worker2);
	std::unique_lock<std::mutex> lk(m);
	cv.wait(lk, [] { return done; });  // ���� data.wait() �̶� ���� �ȴ�.
	lk.unlock();

	std::cout << "Step2 ���� ������ : " << info << std::endl;
	tr2.join();

	//Step 3 ���� ���� ----------------------------------------------------------------------------------------
	std::promise<string> p3;
	std::future<string> data3 = p3.get_future(); // �̷��� string �����͸� ���� �ְڴٴ� ���.

	std::thread tr3(worker3, &p3);
	data3.wait(); // �̷��� ��ӵ� �����͸� ���� �� ���� ��ٸ���.

	//�̷��� future �� ���޵� ���� ��ü�� ������ ���� get �Լ��� ȣ���Ͽ��� ��, 
	//������ future �� ���޵� ���� ��ü�� ��������, ��ġ try �� catch ���� ����� ��ó�� ���ܸ� ó���� �� �ְ� �ȴ�.
	try {
		data3.get();
	}
	catch (const std::exception& e) {
		std::cout << "Step3 ���� : " << e.what() << std::endl;
	}
	tr3.join();

	//Step 4 wait_for ���� -------------------------------------------------------------------------------------
	// void �� ��� ��� ��ü�� �������� ������, future �� set �� �Ǿ��� �ȵǾ������� ������ ��ġ �÷����� ������ ������ �� �ִ�.
	std::promise<void> p4;
	std::future<void> data4 = p4.get_future(); // �̷��� string �����͸� ���� �ְڴٴ� ���.
	std::thread tr4(worker4, &p4);

	std::cout << "Step4 : ";
	// �̷��� ��ӵ� �����͸� ���� �� ���� ��ٸ���.
	while (true) {
		//wait_for �Լ��� promise �� ������ �� ���� ��ٸ��� ��ſ� wait_for �� ���޵� �ð� ��ŭ ��ٷȴٰ� �ٷ� �����ع�����.
		//�� �� �����ϴ� ���� ���� future �� ���¸� ��Ÿ���� future_status ��ü�� �ȴ�.

		//future_status �� �� 3 ���� ���¸� ���� �� �ִ�.
		//1. future_status::ready - future �� ���� ���� �Ǿ��� �� ��Ÿ���� ���� 
		//2. future_status::timeout - wait_for �� ������ �ð��� �������� ���� �������� ���� ����
		//3. future_status::deferred - ������� ����ϴ� �Լ��� ä ������� �ʾҴٴ� �ǹ�
		std::future_status status = data4.wait_for(std::chrono::seconds(1));
		
		if		(status == std::future_status::timeout) { std::cerr << ">"; } // ���� �غ� �ȵǾ��ٸ� ">"�� ���
		else if (status == std::future_status::ready) { break; } // promise �� future �� ������.
	}
	tr4.join();
	std::cout << std::endl;

	//Step 5 shared_future ���� -------------------------------------------------------------------------------------
	std::promise<void> p5;
	std::shared_future<void> start = p5.get_future();

	std::thread run1(runner, &start);
	std::thread run2(runner, &start);
	std::thread run3(runner, &start);
	std::thread run4(runner, &start);

	// ����� cerr �� std::cout ���� �ٸ��� ���۸� ������� �ʱ� ������ �͹̳ο� �ٷ� ��µȴ�.
	// ���� �����忡�� �޽����� ����� �� cerr�� ����ϴ� ���� ����.
	std::cerr << "Step5 : �غ�...";
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cerr << "��!" << std::endl;

	p5.set_value();

	run1.join(); run2.join(); run3.join(); run4.join();

	//Step 6 std::async �񵿱� ó�� ���� ------------------------------------------------------------------------------
	std::vector<int> vec;
	vec.reserve(1000);
	for (int i = 0; i < 1000; ++i) { vec.push_back(i + 1); }

	std::cout << "Step 6: 1 ���� 1000 ������ �� : " << parallel_sum(vec) << std::endl;
}

//.get()�� ������尡 �� ũ��. ���� future�� thread�� ���� �ʰ� 
//.wait_for() ������ �ֱ������� üũ�ϸ鼭 �� ���� �ٸ� ��׶��� �۾��� �ϴ� �� ȿ�����̴�.