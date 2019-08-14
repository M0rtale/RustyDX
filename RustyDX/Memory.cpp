#include "Memory.h"


Memory *Memory::_This = nullptr;

static DWORD GetPIDForProcess(const char *Str)
{
	BOOL            working = 0;
	PROCESSENTRY32 lppe = { 0 };
	DWORD            targetPid = 0;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot)
	{
		lppe.dwSize = sizeof(lppe);
		working = Process32First(hSnapshot, &lppe);
		while (working)
		{
			if (std::string(lppe.szExeFile) == Str)
			{
				targetPid = lppe.th32ProcessID;
				break;
			}
			working = Process32Next(hSnapshot, &lppe);
		}
	}

	CloseHandle(hSnapshot);
	return targetPid;
}


bool Memory::Init()
{
	LONG64 PID = GetPIDForProcess("RustClient.exe");
	if (PID == 0)
	{
		MessageBoxA(NULL, "ERROR-Process!", NULL, NULL);
		exit(0);
	}
	_Handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, PID);
	HKEY phkResult;
	if (ERROR_SUCCESS == RegOpenKeyA(HKEY_LOCAL_MACHINE, "SYSTEM\\ControlSet001\\Services\\DdiMon", &phkResult))
	{
		ULONG64 temp_value1 = 0;
		DWORD lpcbData1 = 8;
		RegQueryValueExA(phkResult, "PID", NULL, NULL, (LPBYTE)& temp_value1, &lpcbData1);
		if (temp_value1 != 0)
		{
			RegCloseKey(phkResult);
			printf("Base Address: %llu\n", temp_value1);
			Base = temp_value1;
			return true;
		}

		ULONG64 temp_PID = PID;
		RegSetValueExA(phkResult, "PID", 0, REG_QWORD, (LPBYTE)& temp_PID, sizeof(ULONG64));

		//force driver to update value
		char p[100] = { 0 };
		SIZE_T temp;
		ReadProcessMemory(_Handle, (void*)0x400000, p, 1, &temp);

		ULONG64 temp_value;
		DWORD lpcbData = 8;

		RegQueryValueExA(phkResult, "PID", NULL, NULL, (LPBYTE)& temp_value, &lpcbData);
		Base = temp_value;
		printf("Base Address: %llu\n", temp_value);

		RegCloseKey(phkResult);
		return true;
	}
	return false;
}


bool Memory::Read_Memory(ULONG64 Addr, void *Buffer, ULONG64 Size)
{
	return ReadProcessMemory(_Handle, (LPVOID)Addr, Buffer, Size, NULL);
}

bool Memory::Write_Memory(ULONG64 Addr, void *Buffer, ULONG64 Size)
{
	return WriteProcessMemory(_Handle, (LPVOID)Addr, &Buffer, Size, NULL);
}

void* Memory::Alloc_Memory(ULONG64 Size)
{
	return false;
}

ULONG64 Memory::Get_Model_Addr()
{
	return Base;
}