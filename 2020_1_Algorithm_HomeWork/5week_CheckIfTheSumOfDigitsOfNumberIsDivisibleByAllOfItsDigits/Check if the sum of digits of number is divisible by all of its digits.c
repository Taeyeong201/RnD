#include <stdio.h>

int main() {
	int N, tmp, sum;
	char digits[64];

	N = tmp = sum = 0;

	printf("Input: N = ");
	scanf_s("%u", &N);

	while (N) {
		sum += digits[tmp++] = N % 10;
		N /= 10;
	}

	for (int i = 0; i < tmp; i++) {
		if ((sum % digits[i]) != 0) {
			tmp = 0;
			break;
		}
	}

	if (tmp == 0) {
		printf("Ouput: NO");
	}
	else {
		printf("Ouput: YES");
	}

	return 0;
}