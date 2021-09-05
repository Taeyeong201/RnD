#include<stdio.h>
#include<time.h>

void move(int disk_num, char src, char dst);
void Hanoi(int numOfDisk, char src, char use, char dst);

int numOfDisk = 0;

int main()
{
	printf("원판 개수 : ");
	scanf_s("%d", &numOfDisk);
	clock_t start = clock();
	Hanoi(numOfDisk, 'A', 'B', 'C');
	clock_t stop = clock();
	double duration = ((double)(stop - start)) / CLOCKS_PER_SEC;
	printf("%lf sec\n", duration);
	return 0;
}
void move(int disk_num, char src, char dst)
{
	static int step = 1;
	printf("step %d - %2d번 원판 %c -> %c\n", step++, numOfDisk - disk_num + 1, src, dst);
}

void Hanoi(int n, char src, char use, char dst)
{
	if (n <= 0)
	{
		return 0;
	}
	else
	{
		Hanoi(n - 1, src, dst, use);
		move(n, src, dst);
		Hanoi(n - 1, use, src, dst);
	}
}