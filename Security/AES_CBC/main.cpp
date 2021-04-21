
#include <iostream>

extern "C" 
{
#include "aes.h"
}


uint8_t key[] = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
                  0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };
uint8_t iv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

#define BUF_SIZE 10240

int main() {
	std::cout << "AES 256 TEST" << std::endl;


    std::string plainText;
    std::cout << "Input Text : ";
    std::cin >> plainText;

    int textSize = plainText.size();
    textSize;

    std::cout << "original Text : ";
    for (int i = 0; i < textSize; i++) {
        printf("0x%.2x ", plainText.at(i));
    }
    printf("\n");

    unsigned char textBuffer[BUF_SIZE] = { 0, };

    plainText.copy((char *)textBuffer, textSize, 0);

    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, textBuffer, textSize);
    
    std::cout << "cipher buffer : ";
    for (int i = 0; i < textSize; i++) {
        printf("0x%.2x ", textBuffer[i]);
    }
    printf("\n");

    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, textBuffer, textSize);

    std::cout << "plain buffer : ";
    for (int i = 0; i < textSize; i++) {
        printf("0x%.2x ", textBuffer[i]);
    }
    printf("\n");

    std::string last((char*)textBuffer, textSize);
    std::cout << "Output text : " << last << std::endl;


	return 0;
}