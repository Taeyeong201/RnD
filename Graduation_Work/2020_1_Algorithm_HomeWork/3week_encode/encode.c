#include <stdio.h>
#include <string.h>

void encode(char *string, int length) {
    int count = 0, compress = 0, rate;
    char result[4096] = "", buffer[8];
    
    for (int i = 0; i < length; i++)
    {
        count++;
        if (string[i] != string[i + 1])
        {
            if (count == 1) {
                sprintf(buffer, "%c", string[i]);
                compress++;
            }
            else {
                sprintf(buffer, "%c%d", string[i], count);
                compress += 2;
            }
            strcat(result, buffer);
            count = 0;
        }
    }
    
    if (strlen(result) >= strlen(string)) {
        printf("압축없음 \norigin length %d, %s\nencode length %d, %s\n",
            strlen(string), string, strlen(result), result);
    }
    else {
        rate = (int)(((double)compress / (double)length) * 100);
		printf("%d%% | %d%% 압축 %s\n", rate, rate - 100, result);
        printf("origin length %d, %s\nencode length %d, %s\n",
            strlen(string), string, strlen(result), result);
    }

    return ;
}

int main()
{
    char arr[4096];
    scanf_s("%s", arr, sizeof(arr));
	encode(arr, strlen(arr));

    return 0;
}