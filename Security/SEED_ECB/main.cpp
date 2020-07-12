
#include <iostream>
#include <iomanip>

#include "SEED_ECB.h"

#define BUFFERSIZE 16

unsigned char pbUserKey[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 		// User secret key
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void TEST1() {
	// TEST 1 - BYTE Buffer
	int i;
	std::cout << "### TEST 1 - BYTE Buffer ###\n" << std::endl;
	unsigned char pbData[16] = {
		0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F };
	unsigned char* cipherText = nullptr;
	unsigned char* decrypText = nullptr;
	SEED_ECB test(pbUserKey);
	test.printRoundKey();
	// Print plaintext to be encrypted
	std::cout << "PlainText  : ";
	for (i = 0; i < BUFFERSIZE; i++)
		printf("%02X ", pbData[i]);

	// Encryption
	std::cout << "\n\nEncryption...." << std::endl;
	int size = test.SEED_ECB_Process(pbData, cipherText, BUFFERSIZE);

	// print encrypted data(ciphertext)
	std::cout << "CipherText size : " << size << std::endl;
	std::cout << "CipherText : ";
	for (i = 0; i < size; i++)
		printf("%02X ", cipherText[i]);

	// Decryption
	std::cout << "\n\nDecryption...." << std::endl;
	test.changeMode(SEED_ECB::ENC_DEC::DECRYPT);
	test.SEED_ECB_Process(cipherText, decrypText, size);

	// Print decrypted data(plaintext)
	std::cout << "PlainText  : ";
	for (i = 0; i < BUFFERSIZE; i++)
		printf("%02X ", decrypText[i]);

	std::cout << std::endl;

	delete cipherText;
	delete decrypText;
};

void TEST2() {
	// TEST 1 - BYTE Buffer
	int i;
	std::cout << "### TEST 2 - BYTE Buffer ###" << std::endl;

	std::string plainText("안녕하세요요요요용요용요요용요용요요용요용요용요용요용요용요요용요용요");
	unsigned char buffer[512];
	// OR 
	// unsigned char buffer = new buffer[plainText.size()+1];

	unsigned char* cipherText2 = nullptr;
	unsigned char* decrypText2 = nullptr;

	SEED_ECB test2(pbUserKey);

	strcpy_s((char*)buffer, plainText.size()+1, plainText.c_str());
	std::cout << "PlainText  : " << std::string((char*)buffer) << std::endl;;


	std::cout << "\n\nEncryption...." << std::endl;
	int size2 = test2.SEED_ECB_Process(buffer, cipherText2, plainText.length()+1);

	std::string cipherString((char*)cipherText2);
	std::cout << "CipherText size : " << size2 << std::endl;
	std::cout << "CipherText : " << cipherString << std::endl;;
	//for (i = 0; i < size2; i++)
	//	printf("%c", cipherText2[i]);

	std::cout << "\n\nDecryption...." << std::endl;
	test2.changeMode(SEED_ECB::ENC_DEC::DECRYPT);
	int size3 = test2.SEED_ECB_Process(cipherText2, decrypText2, size2);

	std::cout << "PlainText  : " << std::string((char*)decrypText2) << std::endl;;
	//for (i = 0; i < size3; i++)
	//	printf("%c", decrypText2[i]);

	std::cout << std::endl;

	delete cipherText2;
	delete decrypText2;
};

int main() {



	// TEST 1 - BYTE Buffer
	TEST1();

	std::cout << "##################" << std::endl;
	std::cout << "Press Enter to start next.\n" << std::endl;;
	getchar();

	// TEST 2 - string
	TEST2();
	getchar();

	std::cout << std::endl;

	return 0;
}