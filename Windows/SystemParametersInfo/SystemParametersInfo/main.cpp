// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-systemparametersinfoa

#include <windows.h>
#include <stdio.h>
#pragma comment(lib, "user32.lib")    

void main()
{
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


		//if (MKF_MOUSEMODE & aMouseInfo.dwFlags) {
		//	printf("on MKF_MOUSEMODE\n");
		//	aMouseInfo.dwFlags -= MKF_MOUSEMODE;
		//	fResult = SystemParametersInfo(SPI_SETMOUSEKEYS,
		//		sizeof(MOUSEKEYS),
		//		&aMouseInfo,
		//		0);
		//	if (fResult)
		//		printf("off MKF_MOUSEMODE\n");
		//	else
		//		printf("off fail\n");
		//}
		//else {
		//	aMouseInfo.dwFlags |= MKF_MOUSEMODE;
		//	fResult = SystemParametersInfo(SPI_SETMOUSEKEYS,
		//		sizeof(MOUSEKEYS),
		//		&aMouseInfo,
		//		0);
		//	if (fResult)
		//		printf("on MKF_MOUSEMODE\n");
		//	else
		//		printf("on fail\n");
		//}
	}
	else {
		printf("get info fail\n");
	}

	system("pause");

	// Double it.         
		//if (fResult)
		//{
		//    aMouseInfo[2] = 2 * aMouseInfo[2];

		//    // Change the mouse speed to the new value.
		//    SystemParametersInfo(SPI_SETMOUSE,      // Set mouse information
		//        0,                 // Not used
		//        aMouseInfo,        // Mouse information
		//        SPIF_SENDCHANGE);  // Update Win.ini
		//}



}