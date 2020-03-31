#include <stdio.h>

#define RESULT_PRINT(String, case, result) \
	printf(#String" #%d\n%d\n", case, result);

int main() {

	unsigned uCaseCount;

	scanf_s("%d", &uCaseCount);
	uCaseCount++;

	for (int uCase = 1; uCase < uCaseCount; uCase++)
	{
		int iResult = 0;
		unsigned int iNumCount, iInput;

		scanf_s("%u", &iNumCount);

		for (int i = 0; i < iNumCount; i++) {
			scanf_s("%u", &iInput);
			iResult ^= iInput;
		}

		RESULT_PRINT(Case, uCase, iResult);
	}
	return 0;
}