#include "FileCrypto.h"

//#include <Windows.h>

int fs_decrypt_aes(char *in_file, char *out_file)
{
	const unsigned char key32[32] =
	{
	0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,
	0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,0x12,
	0x56,0x78,0x9a,0xbc,0xde,0xf0,0x12,0x34,
	0x78,0x9a,0xbc,0xde,0xf0,0x12,0x34,0x56
	};

	AES_KEY aes_ks3;
	unsigned char iv[IV_SIZE];

	unsigned char buf[FREAD_COUNT + BLOCK_SIZE];
	int len = 0;
	int total_size = 0;
	int save_len = 0;
	int w_len = 0;

	FILE *fp = fopen(in_file, "rb");
	if (fp == NULL) {
		fprintf(stderr, "[ERROR] %d can not fopen('%s')\n", __LINE__, in_file);
		return FAIL;
	}

	FILE *wfp = fopen(out_file, "wb");
	if (wfp == NULL) {
		fprintf(stderr, "[ERROR] %d can not fopen('%s')\n", __LINE__, out_file);
		return FAIL;
	}

	memset(iv, 0, sizeof(iv)); // the same iv
	AES_set_decrypt_key(key32, KEY_BIT, &aes_ks3);

	fseek(fp, 0, SEEK_END);
	total_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	printf("total_size %d\n", total_size);

	while (len = fread(buf, RW_SIZE, FREAD_COUNT, fp)) {
		if (FREAD_COUNT == 0) {
			break;
		}
		save_len += len;
		w_len = len;

		AES_cbc_encrypt(buf, buf, len, &aes_ks3, iv, AES_DECRYPT);
		if (save_len == total_size) { // check last block
			w_len = len - buf[len - 1];
			printf("dec padding size %d\n", buf[len - 1]);
		}

		fwrite(buf, RW_SIZE, w_len, wfp);
	}

	fclose(wfp);
	fclose(fp);

	return SUCC;
}

int fs_encrypt_aes(char *in_file, char *out_file)
{
	const unsigned char key32[32] =
	{
	0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,
	0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,0x12,
	0x56,0x78,0x9a,0xbc,0xde,0xf0,0x12,0x34,
	0x78,0x9a,0xbc,0xde,0xf0,0x12,0x34,0x56
	};

	AES_KEY aes_ks3;
	unsigned char iv[IV_SIZE];

	int i = 0;
	int len = 0;
	int padding_len = 0;
	unsigned char buf[FREAD_COUNT + BLOCK_SIZE];

	FILE *fp = fopen(in_file, "rb");
	if (fp == NULL) {
		fprintf(stderr, "[ERROR] %d can not fopen('%s')\n", __LINE__, in_file);
		return FAIL;
	}

	FILE *wfp = fopen(out_file, "wb");
	if (wfp == NULL) {
		fprintf(stderr, "[ERROR] %d can not fopen('%s')\n", __LINE__, out_file);
		return FAIL;
	}

	memset(iv, 0, sizeof(iv)); // init iv
	AES_set_encrypt_key(key32, KEY_BIT, &aes_ks3);
	while (len = fread(buf, RW_SIZE, FREAD_COUNT, fp)) {
		if (FREAD_COUNT != len) {
			break;
		}

		AES_cbc_encrypt(buf, buf, len, &aes_ks3, iv, AES_ENCRYPT);
		fwrite(buf, RW_SIZE, len, wfp);
	}


	// padding  : pkcs5? pkcs7?? http://wiki.dgoon.net/doku.php?id=ssl:pkcs_5
	padding_len = BLOCK_SIZE - len % BLOCK_SIZE;
	printf("enc padding len:%d\n", padding_len);
	memset(buf + len, padding_len, padding_len);
	/**
		for(i=len; i < len+padding_len ;i++){
			buf[i]=padding_len;
		}
	**/
	AES_cbc_encrypt(buf, buf, len + padding_len, &aes_ks3, iv, AES_ENCRYPT);
	fwrite(buf, RW_SIZE, len + padding_len, wfp);

	fclose(wfp);
	fclose(fp);

	return SUCC;
}

int fs_encrypt_seed(char* in_file, char* out_file)
{
	const unsigned char key16[16] =
	{
	0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,
	0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,0x12
	};
	SEED_KEY_SCHEDULE cbc_ks;
	unsigned char iv[IV_SIZE];

	int i = 0;
	int len = 0;
	int padding_len = 0;
	unsigned char buf[FREAD_COUNT + BLOCK_SIZE];

	FILE* fp = fopen(in_file, "rb");
	if (fp == NULL) {
		fprintf(stderr, "[ERROR] %d can not fopen('%s')\n", __LINE__, in_file);
		return FAIL;
	}

	FILE* wfp = fopen(out_file, "wb");
	if (wfp == NULL) {
		fprintf(stderr, "[ERROR] %d can not fopen('%s')\n", __LINE__, out_file);
		return FAIL;
	}

	memset(iv, 0, sizeof(iv)); // init iv
	SEED_set_key(key16, &cbc_ks);
	while (len = fread(buf, RW_SIZE, FREAD_COUNT, fp)) {
		if (FREAD_COUNT != len) {
			break;
		}

		SEED_cbc_encrypt(buf, buf, len, &cbc_ks, iv, SEED_ENCRYPT);
		fwrite(buf, RW_SIZE, len, wfp);
	}


	// padding  : pkcs5? pkcs7?? http://wiki.dgoon.net/doku.php?id=ssl:pkcs_5
	padding_len = BLOCK_SIZE - len % BLOCK_SIZE;
	printf("enc padding len:%d\n", padding_len);
	memset(buf + len, padding_len, padding_len);
	/**
		for(i=len; i < len+padding_len ;i++){
			buf[i]=padding_len;
		}
	**/
	SEED_cbc_encrypt(buf, buf, len + padding_len, &cbc_ks, iv, SEED_ENCRYPT);
	fwrite(buf, RW_SIZE, len + padding_len, wfp);

	fclose(wfp);
	fclose(fp);

	return SUCC;
}

int fs_decrypt_seed(char* in_file, char* out_file)
{
	const unsigned char key16[16] =
	{
	0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,
	0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,0x12
	};
	SEED_KEY_SCHEDULE cbc_ks;
	unsigned char iv[IV_SIZE];

	unsigned char buf[FREAD_COUNT + BLOCK_SIZE];
	int i = 0;
	int len = 0;
	int total_size = 0;
	int save_len = 0;
	int w_len = 0;

	FILE* fp = fopen(in_file, "rb");
	if (fp == NULL) {
		fprintf(stderr, "[ERROR] %d can not fopen('%s')\n", __LINE__, in_file);
		return FAIL;
	}

	FILE* wfp = fopen(out_file, "wb");
	if (wfp == NULL) {
		fprintf(stderr, "[ERROR] %d can not fopen('%s')\n", __LINE__, out_file);
		return FAIL;
	}

	memset(iv, 0, sizeof(iv)); // the same iv
	SEED_set_key(key16, &cbc_ks);

	fseek(fp, 0, SEEK_END);
	total_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	printf("total_size %d\n", total_size);

	while (len = fread(buf, RW_SIZE, FREAD_COUNT, fp)) {
		if (FREAD_COUNT == 0) {
			break;
		}
		save_len += len;
		w_len = len;

		SEED_cbc_encrypt(buf, buf, len, &cbc_ks, iv, SEED_DECRYPT);
		if (save_len == total_size) { // check last block
			w_len = len - buf[len - 1];
			printf("dec padding size %d\n", buf[len - 1]);
		}

		fwrite(buf, RW_SIZE, w_len, wfp);
	}

	fclose(wfp);
	fclose(fp);

	return SUCC;
}



/*
	wchar to char
	char to wchar
*/
char * ConvertWCtoC(wchar_t* str)
{
	//반환할 char* 변수 선언
	char* pStr;

	//입력받은 wchar_t 변수의 길이를 구함
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);

	//char* 메모리 할당
	pStr = new char[strSize];

	//형 변환 
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);

	return pStr;
}
wchar_t* ConverCtoWC(char* str)
{
	//wchar_t형 변수 선언
	wchar_t* pStr;
	//멀티 바이트 크기 계산 길이 반환
	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);

	//wchar_t 메모리 할당
	pStr = new WCHAR[strSize];
	//형 변환
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);

	return pStr;
}
