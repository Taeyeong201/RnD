

#include <iostream>
#include <Windows.h>
#include <Tlhelp32.h>
#include <tchar.h>
#include <atlstr.h>

BOOL KillProcess2()
{
	HANDLE hProcessSnap = NULL;
	DWORD Return = FALSE;
	PROCESSENTRY32 pe32 = { 0 };

	CString ProcessName = "vmon.exe";
	ProcessName.MakeLower();

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return (DWORD)INVALID_HANDLE_VALUE;

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcessSnap, &pe32))
	{
		DWORD Code = 0;
		DWORD dwPriorityClass;

		do {
			HANDLE hProcess;
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
			dwPriorityClass = GetPriorityClass(hProcess);

			CString Temp = pe32.szExeFile;
			Temp.MakeLower();

			if (Temp == ProcessName)
			{
				if (TerminateProcess(hProcess, 0))
					GetExitCodeProcess(hProcess, &Code);
				else
					return GetLastError();
			}
			CloseHandle(hProcess);
		} while (Process32Next(hProcessSnap, &pe32));
		Return = TRUE;
	}
	else
		Return = FALSE;

	CloseHandle(hProcessSnap);

	return Return;
}

void KillProcess(const char* EXEName)
{
	HANDLE snapshot_handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE != snapshot_handle)
	{
		PROCESSENTRY32 pe;
		if (Process32First(snapshot_handle, &pe))
		{
			do
			{
				//TRACE("KillProcess() PID = %04u, FileName = %s\n", pe.th32ProcessID, pe.szExeFile);
				if (!_tcscmp(pe.szExeFile, (LPCTSTR)EXEName))
				{
					HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
					if (INVALID_HANDLE_VALUE != process_handle)
					{
						//PLOG_INFO << "kill app(" << EXEName << ") : OK";
						std::cout << "kill app(" << EXEName << ") : OK" << std::endl;
						TerminateProcess(process_handle, 0);
						CloseHandle(process_handle);
					}
				}
			} while (Process32Next(snapshot_handle, &pe));
		}

		CloseHandle(snapshot_handle);
		snapshot_handle = INVALID_HANDLE_VALUE;
	}
}

int main() {

	KillProcess2();
	std::cout << "kill app : OK" << std::endl;

	getchar();
	return 0;
}