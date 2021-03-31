#include <iostream>
#include <mfapi.h>

int main() {
	HRESULT hr = MFStartup(MF_VERSION);


	MFShutdown();


	system("pause");

	return 0;
}