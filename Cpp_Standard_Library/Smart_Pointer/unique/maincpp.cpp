

#include <iostream>
#include <memory>
#include <cassert>

/*
	스마트 포인터 중 가장 많이 사용되는 것이 바로 유니크 포인터이다. 
	유니크 포인터는 다음과 같은 특징을 가진다.
		- 원시 포인터(소유 중인 포인터)의 소유자는 한 명이다.
		- 원시 포인터에 대한 소유권을 이전할 순 있지만 복사나 대입과 같은 공유를 불허한다.
		- 유니크 포인터 객체가 소멸될 때 원시 포인터도 소멸된다.
*/


int main() {
	auto u_ptr(std::make_unique<unsigned char[]>(4096));
	/*
		unique_ptr의 멤버 함수인 get 을 통해 다음과 같이 원시 포인터를 반환할 수 있다.
		unsigned char* f = u_ptr.get();    // 위험할 수 있음
		원시 포인터의 소유권을 포기하고 반환하는 것이 아니라 
		소유하고 있는 채로 반환하는 것이기 때문에 위험한 결과를 초래할 수 있다
	*/
	const unsigned char* f = u_ptr.get(); // 그나마 안전, 하지만 여전히 delete 가능
	//원시 포인터 해체 및 재설정
	//u_ptr.reset(new unsigned char[1024]);
	u_ptr.reset(); //해제

	auto test = u_ptr[1];

	auto u_ptr2 = std::make_unique<int>();
	*u_ptr2 = 25;


	//소유권 이전
	//std::unique_ptr<int> unique_Int = std::move(u_ptr2);
	/*
		유니크 포인터가 관리해주던 메모리를 
		C스타일의 포인터로 받기 때문에 책임지고 사용 종료 시 해제해주어야 한다.
	*/
	int* pInt = u_ptr2.release();

	if (u_ptr2) { assert(*u_ptr2 == 25); } // 실행되지 않음
	if (pInt) { assert(*pInt == 25); } // 실행 됨
	delete pInt;

	return 0;
}