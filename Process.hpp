#pragma once
#include <wtypes.h>
#include <TlHelp32.h>
#include <tchar.h>

int GetProcessThreadNumByID(DWORD dwPID)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		return 0;
	}

	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(pe32);

	BOOL bRet = Process32First(hProcessSnap, &pe32);

	while (bRet)
	{
		if (pe32.th32ProcessID == dwPID)
		{
			CloseHandle(hProcessSnap);

			return pe32.cntThreads;
		}

		bRet = Process32Next(hProcessSnap, &pe32);
	}

	return 0;
}


int GetProcessID(const char* process_name)
{
	DWORD dwRet = 0, dwThreadCountMax = 0;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	Process32First(hSnapshot, &pe32);

	do
	{
		if ((pe32.szExeFile, process_name) == 0)

		{
			DWORD dwTmpThreadCount = GetProcessThreadNumByID(pe32.th32ProcessID);

			if (dwTmpThreadCount > dwThreadCountMax)
			{
				dwThreadCountMax = dwTmpThreadCount;
				dwRet = pe32.th32ProcessID;
			}
		}
	} while (Process32Next(hSnapshot, &pe32));

	CloseHandle(hSnapshot);

	return dwRet;
}