//std::promise 와 std::future
//-------------------------------------------------------------------------------------------------------------------------
//<future>는 멀티쓰레드 구조에 대한 구현이라기보다는 비동기적 처리를 용이하게 처리하도록 하는데 목표를 두고 있는
//좀 더 고수준의 멀티쓰레딩 개념을 구현하는 일련의 클래스를 제공한다.
//future는 소비자(구독자)에 의해 사용될 최종 결과물(미래의 산출물)이고, promise는 생산자(제작자)가 사용하는 개념이다.

//프로그램의 실행이, 한 갈래가 아니라 여러 갈래로 갈라져서 동시에 진행되는 것을 비동기적(asynchronous) 실행 이라고 부른다.
//결국 비동기적 실행으로 하고 싶은 일은, 어떠한 데이터를 다른 쓰레드를 통해 처리해서 받아내는 것이다.
//내가 어떤 쓰레드 T 를 사용해서, 비동기적으로 값을 받아내겠다 라는 의미는, 
//미래에(future) 쓰레드 T 가 원하는 데이터를 돌려 주겠다 라는 약속(promise) 이라고 볼 수 있다.

/* 
약속 객체는 태스크 결과를 자료 채널에 밀어 넣고, 미래 객체는 작업이 끝나길 기다렸다가 자료 채널에서 결과를 꺼낸다.
+-----------------+                 +----------+                 +-----------------+
| promise(송신자) | ===== set ====> |   채널   | ===== get ====> | future(수신자)  |
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
	// 약속을 이행하는 모습. 해당 결과는 future 에 들어간다.
	// promise 객체가 자신의 future 객체에 데이터를 제공한 후에 wait 해야 한다.
	p->set_value("some data");
}
//------------------------------------------------------

// Step 2
//생산자-소비자 패턴에서 promise 는 마치 생산자(producer)의 역할을 수행하고, future 는 소비자(consumer)의 역할을 수행한다. 
//그러나, promise 와 future 를 이용하는 것이 훨씬 더 깔끔하고 더 이해하기도 쉽다. 
//또한 조건 변수를 사용한 것 보다 더 우수한 점은 future 에 예외도 전달할 수 있기 때문이다.

std::condition_variable cv;
std::mutex m;
bool done = false;
std::string info;

void worker2() { //조건 변수 사용
	{
		std::lock_guard<std::mutex> lk(m);
		info = "some data";  // 위의 p->set_value("some data") 에 대응
		done = true;
	}
	cv.notify_all();
}
//------------------------------------------------------

// Step3 future & promise에 예외 전달 사용
void worker3(std::promise<string>* p) {
	try {
		throw std::runtime_error("Some Error!");
	}
	catch (...) {
		//set_exception 에는 exception_ptr 를 전달해야 한다. exception_ptr 는 catch 로 받은 예외 객체의 포인터가 아니라, 
		//현재 catch 된 예외에 관한 정보를 반환하는 current_exception 함수가 리턴하는 객체이기 때문이다.
		//물론, catch 로 전달받은 예외 객체를 make_exception_ptr 함수를 사용해서 exception_ptr 로 만들 수 도 있지만, 
		//그냥 편하게 current_exception 을 호출하는 것이 더 간단하다.
		p->set_exception(std::current_exception());
	}
}
//------------------------------------------------------

// Step4 future & promise에 wait_for 적용
void worker4(std::promise<void>* p) {
	std::this_thread::sleep_for(std::chrono::seconds(10));
	p->set_value();
}
//------------------------------------------------------

// Step5 shared_future
//future 의 경우 get 을 호출하면 future 내부의 객체가 이동되기 때문에 get 을 딱 한 번만 할 수 있다
//하지만, 종종 여러 개의 다른 쓰레드에서 future 를 get 할 필요성이 있다. 이 때 shared_future 를 사용하면 된다.
//아래 예제는 달리기를 하는 것을 C++ 프로그램으로 나타내본 것이다. 
//main 함수에서 출발 신호를 보내면 각 runner 쓰레드들에서 달리기를 시작하게 된다.
void runner(std::shared_future<void>* start) {
	start->get();
	std::cout << "출발!" << std::endl;
}
//------------------------------------------------------

// Step6 std::async 비동기 처리
//promise 나 packaged_task 는 비동기적으로 실행을 하기 위해서는, 쓰레드를 명시적으로 생성해서 실행해야만 했다. 
//하지만 std::async 에 어떤 함수를 전달한다면, 아예 쓰레드를 알아서 만들어서 해당 함수를 비동기적으로 실행하고, 그 결과값을 future 에 전달한다.
// std::accumulate 와 동일

int sum(const std::vector<int>& v, int start, int end) {
	int total = 0;
	for (int i = start; i < end; ++i) { total += v[i]; }
	return total;
}

/*async 함수는 인자로 받은 함수를 비동기적으로 실행한 후에, 해당 결과값을 보관할 future 를 리턴한다.
첫 번째 인자로는 어떠한 형태로 실행할지를 전달하는데 두 가지 값이 가능하다.
1. std::launch::async : 바로 쓰레드를 생성해서 인자로 전달된 함수를 실행한다. 즉, 바로 그 자리에서 쓰레드를 생성해서 실행하게 된다.
2. std::launch::deferred : future 의 get 함수가 호출되었을 때 쓰레드를 생성해서 실행하기 때문에 비동기적으로 실행하게 된다.
   다시 말해, 해당 함수를 굳이 바로 당장 비동기적으로 실행할 필요가 없다면 deferred 옵션을 주면 된다. */

/*다음 parallel 함수는 1 부터 1000 까지의 덧셈을 총 2 개의 쓰레드에서 실행한다
1 부터 500 까지의 합은, async 를 통해 생성된 새로운 쓰레드에서 더하게 되고, 
나머지 501 부터 1000 까지의 합은 원래의 쓰레드에서 처리하게 된다. */
int parallel_sum(const std::vector<int>& v) {
	// lower_half_future 는 1 ~ 500 까지 비동기적으로 더함
	// 참고로 람다 함수를 사용하면 좀 더 깔끔하게 표현할 수 도 있다.
	// --> std::async([&v]() { return sum(v, 0, v.size() / 2); });
	std::future<int> lower_half_future = std::async(std::launch::async, sum, cref(v), 0, v.size() / 2);
	//std::future<int> lower_half_future = std::async([&v]() { return sum(v, 0, v.size() / 2); });

	// upper_half 는 501 부터 1000 까지 더함
	int upper_half = sum(v, v.size() / 2, v.size());

	//async 함수는 실행하는 함수의 결과값을 포함하는 future 를 리턴한다. 
	//그리고 async 함수가 리턴한 future 에 get 을 통해 얻어낼 수 있다.
	return lower_half_future.get() + upper_half;
}

int main() {
	//promise 객체를 정의할 때, 연산을 수행 후에 돌려줄 객체의 타입을 템플릿 인자로 받는다.
	std::promise<string> p; //문자열로 받을 것이기 때문에 string 전달

	// 미래에 string 데이터를 돌려 주겠다는 약속. get_future 함수를 통해서 얻을 수 있다.
	//연산이 끝난 다음에 promise 객체는 자신이 가지고 있는 future 객체에 값을 넣어주게 된다. 
	//하지만 data 가 아직은 실제 연산 결과를 포함하고 있는 것은 아니다. 
	//쓰레드에서 data 가 실제 결과를 포함하기 위해서는 "p->set_value("some data")"와 같이 처리해야 한다.
	std::future<string> data = p.get_future();

	// Step 0 ----------------------
	std::cout << "Main Thread => " << std::this_thread::get_id() << std::endl;
	std::future<int> fn = std::async(std::launch::async, asyncFunc, 100);
	if (fn.valid()) std::cout << fn.get() << std::endl;
	if (fn.valid()) fn.get();
	else std::cout << "Invalid ayncFunc" << std::endl;
	// Step 0 ----------------------

	std::thread tr1(worker, &p);	

	//한 가지 중요한 점은 promise 가 future 에 값을 전달하기 전 까지 wait 함수가 기다린다는 점이다.
	//wait 함수가 리턴을 하였다면 get 을 통해서 future 에 전달된 객체를 얻을 수 있다.

	data.wait(); // 미래에 약속된 데이터를 받을 때 까지 기다린다.
	// 따라서, wait이 리턴했다는 것은 future에 데이터가 준비되었다는 의미.
	
	//참고로 굳이 wait 함수를 별도로 호출할 필요없이 get 함수를 바로 호출하더라도 
	//알아서 promise 가 future 에 객체를 전달할 때 까지 기다린 다음에 리턴한다.
	//즉, get 을 호출하면 future 내에 있던 데이터가 이동 된다. 따라서 get 을 절대로 다시 호출하면 안된다.
	
	std::cout << "Step1 받은 데이터 : " << data.get() << std::endl; // 그냥 get 해도 wait 한 것과 동일하다.
	tr1.join();
	
	//Step 2---------------------------------------------------------------------------------------------------
	std::thread tr2(worker2);
	std::unique_lock<std::mutex> lk(m);
	cv.wait(lk, [] { return done; });  // 위의 data.wait() 이라 보면 된다.
	lk.unlock();

	std::cout << "Step2 받은 데이터 : " << info << std::endl;
	tr2.join();

	//Step 3 예외 전달 ----------------------------------------------------------------------------------------
	std::promise<string> p3;
	std::future<string> data3 = p3.get_future(); // 미래에 string 데이터를 돌려 주겠다는 약속.

	std::thread tr3(worker3, &p3);
	data3.wait(); // 미래에 약속된 데이터를 받을 때 까지 기다린다.

	//이렇게 future 에 전달된 예외 객체는 다음과 같이 get 함수를 호출하였을 때, 
	//실제로 future 에 전달된 예외 객체가 던져지고, 마치 try 와 catch 문을 사용한 것처럼 예외를 처리할 수 있게 된다.
	try {
		data3.get();
	}
	catch (const std::exception& e) {
		std::cout << "Step3 예외 : " << e.what() << std::endl;
	}
	tr3.join();

	//Step 4 wait_for 적용 -------------------------------------------------------------------------------------
	// void 의 경우 어떠한 객체도 전달하지 않지만, future 가 set 이 되었냐 안되었느냐의 유무로 마치 플래그의 역할을 수행할 수 있다.
	std::promise<void> p4;
	std::future<void> data4 = p4.get_future(); // 미래에 string 데이터를 돌려 주겠다는 약속.
	std::thread tr4(worker4, &p4);

	std::cout << "Step4 : ";
	// 미래에 약속된 데이터를 받을 때 까지 기다린다.
	while (true) {
		//wait_for 함수는 promise 가 설정될 때 까지 기다리는 대신에 wait_for 에 전달된 시간 만큼 기다렸다가 바로 리턴해버린다.
		//이 때 리턴하는 값은 현재 future 의 상태를 나타내는 future_status 객체가 된다.

		//future_status 는 총 3 가지 상태를 가질 수 있다.
		//1. future_status::ready - future 에 값이 설정 되었을 때 나타나는 상태 
		//2. future_status::timeout - wait_for 에 지정한 시간이 지났지만 값이 설정되지 않은 상태
		//3. future_status::deferred - 결과값을 계산하는 함수가 채 실행되지 않았다는 의미
		std::future_status status = data4.wait_for(std::chrono::seconds(1));
		
		if		(status == std::future_status::timeout) { std::cerr << ">"; } // 아직 준비가 안되었다면 ">"를 출력
		else if (status == std::future_status::ready) { break; } // promise 가 future 를 설정함.
	}
	tr4.join();
	std::cout << std::endl;

	//Step 5 shared_future 적용 -------------------------------------------------------------------------------------
	std::promise<void> p5;
	std::shared_future<void> start = p5.get_future();

	std::thread run1(runner, &start);
	std::thread run2(runner, &start);
	std::thread run3(runner, &start);
	std::thread run4(runner, &start);

	// 참고로 cerr 는 std::cout 과는 다르게 버퍼를 사용하지 않기 때문에 터미널에 바로 출력된다.
	// 따라서 쓰레드에서 메시지를 출력할 때 cerr를 사용하는 것이 좋다.
	std::cerr << "Step5 : 준비...";
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cerr << "땅!" << std::endl;

	p5.set_value();

	run1.join(); run2.join(); run3.join(); run4.join();

	//Step 6 std::async 비동기 처리 적용 ------------------------------------------------------------------------------
	std::vector<int> vec;
	vec.reserve(1000);
	for (int i = 0; i < 1000; ++i) { vec.push_back(i + 1); }

	std::cout << "Step 6: 1 부터 1000 까지의 합 : " << parallel_sum(vec) << std::endl;
}

//.get()은 오버헤드가 꽤 크다. 따라서 future나 thread가 놀지 않게 
//.wait_for() 등으로 주기적으로 체크하면서 그 동안 다른 백그라운드 작업을 하는 게 효율적이다.