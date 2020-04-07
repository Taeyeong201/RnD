#include <stdio.h>
#include <string.h>


int main() {
	char input[64];
	printf("Input : ");
	scanf_s("%s", input, sizeof(input));

	for (int i = 0; i < strlen(input); i++) {
		if (input[i] == 'g' || input[i] == 'G') {
			if (i % 2) {
				printf("Ouput : Second\n");
			}
			else {
				printf("Ouput : First\n");
			}
			break;
		}
	}

}