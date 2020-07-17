#ifndef __MSLM_CRYPTO_H__
#define __MSLM_CRYPTO_H__

#include <stdio.h>
#include <string.h> 
#include <Windows.h>
#include <openssl/aes.h>
#include <openssl/seed.h>

///////////////// 파일 암호화/복호화
#define BLOCK_SIZE 16
#define FREAD_COUNT 4096
#define KEY_BIT 256
#define IV_SIZE 16
#define RW_SIZE 1
#define SUCC 0
#define FAIL -1

#define SEED_ENCRYPT true
#define SEED_DECRYPT false

int fs_encrypt_aes(char *in_file, char *out_file);
int fs_decrypt_aes(char *in_file, char *out_file);
int fs_encrypt_seed(char *in_file, char *out_file);
int fs_decrypt_seed(char *in_file, char *out_file);
char * ConvertWCtoC(wchar_t* str);
wchar_t* ConverCtoWC(char* str);

#endif