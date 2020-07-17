

#include <iostream>
#include <memory>
#include <cassert>

/*
	����Ʈ ������ �� ���� ���� ���Ǵ� ���� �ٷ� ����ũ �������̴�. 
	����ũ �����ʹ� ������ ���� Ư¡�� ������.
		- ���� ������(���� ���� ������)�� �����ڴ� �� ���̴�.
		- ���� �����Ϳ� ���� �������� ������ �� ������ ���糪 ���԰� ���� ������ �����Ѵ�.
		- ����ũ ������ ��ü�� �Ҹ�� �� ���� �����͵� �Ҹ�ȴ�.
*/


int main() {
	auto u_ptr(std::make_unique<unsigned char[]>(4096));
	/*
		unique_ptr�� ��� �Լ��� get �� ���� ������ ���� ���� �����͸� ��ȯ�� �� �ִ�.
		unsigned char* f = u_ptr.get();    // ������ �� ����
		���� �������� �������� �����ϰ� ��ȯ�ϴ� ���� �ƴ϶� 
		�����ϰ� �ִ� ä�� ��ȯ�ϴ� ���̱� ������ ������ ����� �ʷ��� �� �ִ�
	*/
	const unsigned char* f = u_ptr.get(); // �׳��� ����, ������ ������ delete ����
	//���� ������ ��ü �� �缳��
	//u_ptr.reset(new unsigned char[1024]);
	u_ptr.reset(); //����

	auto test = u_ptr[1];

	auto u_ptr2 = std::make_unique<int>();
	*u_ptr2 = 25;


	//������ ����
	//std::unique_ptr<int> unique_Int = std::move(u_ptr2);
	/*
		����ũ �����Ͱ� �������ִ� �޸𸮸� 
		C��Ÿ���� �����ͷ� �ޱ� ������ å������ ��� ���� �� �������־�� �Ѵ�.
	*/
	int* pInt = u_ptr2.release();

	if (u_ptr2) { assert(*u_ptr2 == 25); } // ������� ����
	if (pInt) { assert(*pInt == 25); } // ���� ��
	delete pInt;

	return 0;
}