// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-systemparametersinfoa

#include <windows.h>
#include <stdio.h>
#pragma comment(lib, "user32.lib")

void SetMouseKeys() {
	BOOL fResult;
	MOUSEKEYS aMouseInfo = { 0, };
	aMouseInfo.cbSize = sizeof(MOUSEKEYS);

	fResult = SystemParametersInfo(SPI_GETMOUSEKEYS,
		sizeof(MOUSEKEYS),
		&aMouseInfo,
		0);
	if (fResult) {
		printf("%lu\n", aMouseInfo.dwFlags);
		if (MKF_MOUSEKEYSON & aMouseInfo.dwFlags) {
			printf("on MKF_MOUSEKEYSON\n");
			aMouseInfo.dwFlags -= MKF_MOUSEKEYSON;
			fResult = SystemParametersInfo(SPI_SETMOUSEKEYS,
				sizeof(MOUSEKEYS),
				&aMouseInfo,
				0);
			if (fResult)
				printf("off MKF_MOUSEKEYSON\n");
			else
				printf("off fail\n");
		}
		else {
			aMouseInfo.dwFlags |= MKF_MOUSEKEYSON;
			fResult = SystemParametersInfo(SPI_SETMOUSEKEYS,
				sizeof(MOUSEKEYS),
				&aMouseInfo,
				0);
			if (fResult)
				printf("on MKF_MOUSEKEYSON\n");
			else
				printf("on fail\n");
		}
	}
	else {
		printf("get info fail\n");
	}
}

void SetDpi(int dpi) {
	BOOL fResult;
	MOUSEKEYS aMouseInfo = { 0, };
	aMouseInfo.cbSize = sizeof(MOUSEKEYS);

	fResult = SystemParametersInfo(SPI_GETLOGICALDPIOVERRIDE,
		sizeof(MOUSEKEYS),
		&aMouseInfo,
		0);
}

void main()
{
	SetMouseKeys();

	system("pause");
}