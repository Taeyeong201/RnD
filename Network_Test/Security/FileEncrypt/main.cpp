#include "FileCrypto.h"

#define ENCODE 0

int main(int argc, char* args[])
{

	if (argc != 2) {
		printf("[Usage] %s fs_src_file\n", args[0]);
		return FAIL;
	}

	char outFileName[1024] = { 0, };
	sprintf(outFileName, "encrypt__%s", args[1]);

	
	if (ENCODE)
	{
		if (fs_encrypt_seed(args[1], outFileName) == SUCC) {
			/*fs_decrypt_aes("encryptFile_fs_in.txt", "decryptFile_fs_out.txt");*/
			printf("result:[%s]\n", outFileName);
		}
	}
	else {
		if (fs_decrypt_seed(args[1], outFileName) == SUCC) {
			/*fs_decrypt_aes("encryptFile_fs_in.txt", "decryptFile_fs_out.txt");*/
			printf("result:[%s]\n", outFileName);
		}
	}


	return 0;
}