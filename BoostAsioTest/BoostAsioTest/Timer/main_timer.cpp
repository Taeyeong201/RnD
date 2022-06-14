#include <cstddef>
#include <iostream>
#include "AsyncTimer.h"

int main()
{
	AsyncTimer timer(10);
	timer.StartTimer();
	bool run = true;
	while (run) {
		int num = 0;
		//std::cout << "reset timer = 1, end = 2 -> ";
		std::cin >> num;
		switch (num) {
		case 1:
			timer.ResetTimer();
			break;
		case 2:
			timer.CloseTimer();
			run = false;
			break;
		default:
			break;
		}
	}


	std::cout << "main end" << std::endl;
}