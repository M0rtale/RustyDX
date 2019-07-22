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