#include <iostream>
#include <memory>

/*
	 unique_ptr 또는 shared_ptr은 원시 포인터를 건네받아 내부적으로 참조하게 했다면, 
	 weak_ptr는 원시 포인터 대신 shared_ptr를 건네받아 shared_ptr가 소유한 원시포인터를 참조하도록 한다. 
	 즉 weak_ptr는 unique_ptr 또는 shared_ptr처럼 원시 포인터를 직접적으로 인자로 받을 수 없으며 
	 * 또는 -> 연산자를 통해 포인터처럼 사용할 수도 없다. 
	 weak_ptr는 다음과 같이 미리 생성된 shared_ptr 개체를 대입하여 생성한다.

*/

int main() {
	std::shared_ptr<int> strong_f = std::make_shared<int>();
	std::weak_ptr<int> weak_f = strong_f;
	// std::weak_ptr<int> weak_f(strong_f) // same

	// compile error
	// std::weak_ptr<int> isNotWorking(new int()); 
	/*
		공유 포인터는 강한 참조로 참조하기 때문에 strong ref count가 1 증가할 것이고, 
		약한 포인터는 약한 참조로 참조하기 때문에 weak ref count가 1 증가한다. 
		_Rep는 참조 테이블을 가리키며, 두 개체 모두 동일한 테이블을 공유한다. 


		[강한 참조와 약한 참조의 차이]
			강한 참조와 약한 참조의 차이는 각 참조의 횟수가 메모리 해제에 영향을 미치느냐 안 미치느냐이다. 
			공유 포인터에선 참조 횟수가 0이 되면 알아서 소멸자를 호출하여 메모리를 해제한다고 했다. 
			이때의 참조 횟수는 바로 강한 참조에 대한 이야기였다. 
			즉 약한 참조 횟수는 메모리 해제랑 관련이 없다. 
			약한 참조 횟수가 0이든 1이든 100이든 상관없이 강한 참조 횟수가 0이 되면 원시 포인터의 메모리는 해제된다.
	*/
	/*
		weak_ptr의 멤버 함수 중 lock이라는 멤버 함수가 있는데, 
		이 함수는 해당 원시 포인터를 참조하는 shared_ptr을 만들어서 반환해준다.
		우리는 이 반환된 shared_ptr를 통해 원시 포인터를 사용할 수 있다.
	*/
	std::shared_ptr<int> lockedPtr = weak_f.lock();
	/*
		lock을 하기도 전에 사용자의 실수로 이미 원시 포인터가 소멸된 경우도 있다. 
		또는 멀티 스레드 환경에서 lock을 하려고 하는 순간, 
		다른 공유 포인터들이 모두 소멸되어 원시 포인터가 소멸되었을 수도 있다. 
		즉 weak_ptr가 "expire" 상태일 경우이다. 이때 lock 함수는 empty 상태인 공유 포인터를 반환한다. 

		멀티 스레드 환경에서 반환된 공유 포인터를 다음과 같이 사용할 수 있다.
		shared_ptr<Foo> lockedPtr = weak_f.lock();
		if(lockedPtr)
		{
			....
		}
		// or
		if(!lockedPtr)
		{
			....
		}

	*/
	// 원시 포인터가 소멸되었다면
	// "expired" 상태이므로 true를 반환하고, 
	// 존재한다면 false를 반환한다.
	if (!weak_f.expired()) {}

	return 0;
}