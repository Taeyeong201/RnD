
#include <iostream>
#include <chrono>
#include <thread>

void running(const std::chrono::seconds& sec) {

	std::this_thread::sleep_for(sec);
}

int main() {

	//����
	std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();

	running(std::chrono::seconds(5));

	//����
	std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();

	
	//std::chrono::duration_cast<�ð� ����>(end - start)
	//std::chrono::nanoseconds  // ���� ������. 10����� 1��
	//std::chrono::microseconds // ����ũ�� ������. 100������ 1��
	//std::chrono::milliseconds   // �и� ������. 1000���� 1��
	//std::chrono::seconds        // ��
	//std::chrono::minutes         // ��
	//std::chrono::hours            // ��
	
	std::cout << "�ɸ� �ð� : " <<
		std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count()
		<< " second" << std::endl;

	getchar();

	return 0;
}