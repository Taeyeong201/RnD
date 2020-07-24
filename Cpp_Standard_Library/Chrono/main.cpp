
#include <iostream>
#include <chrono>
#include <thread>

void running(const std::chrono::seconds& sec) {

	std::this_thread::sleep_for(sec);
}

int main() {

	//시작
	std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();

	running(std::chrono::seconds(5));

	//종료
	std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();

	
	//std::chrono::duration_cast<시간 단위>(end - start)
	//std::chrono::nanoseconds  // 나노 세컨드. 10억분의 1초
	//std::chrono::microseconds // 마이크로 세컨드. 100만분의 1초
	//std::chrono::milliseconds   // 밀리 세컨드. 1000분의 1초
	//std::chrono::seconds        // 초
	//std::chrono::minutes         // 분
	//std::chrono::hours            // 시
	
	std::cout << "걸린 시간 : " <<
		std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count()
		<< " second" << std::endl;

	getchar();

	return 0;
}