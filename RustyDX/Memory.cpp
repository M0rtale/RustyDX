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
	/*HANDLE hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, NULL, "CreateFileMappingA");
	if (hMapFile == nullptr)
	{
		MessageBoxA(NULL, "ERROR-Driver!", NULL, NULL);
		exit(0);
	}
	_Hacker_Memory = (Kernel_RW_Memory_Struct*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (_Hacker_Memory != nullptr)
	{
		RtlZeroMemory(_Hacker_Memory, sizeof(Kernel_RW_Memory_Struct));
		return true;
	}
	MessageBoxA(NULL, "ERROR-Driver!", NULL, NULL);
	exit(0);*/





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
		RegQueryValueExA(phkResult, "PID", NULL, NULL, (LPBYTE)&temp_value1, &lpcbData1);
		if (temp_value1 != 0)
		{
			RegCloseKey(phkResult);
			printf("Base Address: %llu\n", temp_value1);
			Base = temp_value1;
			return true;
		}

		ULONG64 temp_PID = PID;
		RegSetValueExA(phkResult, "PID", 0, REG_QWORD, (LPBYTE)&temp_PID, sizeof(ULONG64));

		//force driver to update value
		char p[100] = { 0 };
		SIZE_T temp;
		ReadProcessMemory(_Handle, (void*)0x400000, p, 1, &temp);

		ULONG64 temp_value;
		DWORD lpcbData = 8;

		RegQueryValueExA(phkResult, "PID", NULL, NULL, (LPBYTE)&temp_value, &lpcbData);
		Base = temp_value;
		printf("Base Address: %llu\n", temp_value);

		RegCloseKey(phkResult);
		return true;
	}
	return false;



	//if (!InitMemRW())
	//{
	//	MessageBoxA(NULL, "ERROR-Init!", NULL, NULL);
	//	exit(0);
	//}
	//if (!SetPid(GetPIDForProcess("RustClient.exe")))
	//{
	//	MessageBoxA(NULL, "ERROR-PID!", NULL, NULL);
	//	exit(0);
	//}
	//ULONGLONG addr = 0;
	//if (!GetModBase(L"UnityPlayer.dll", addr))
	//{
	//	MessageBoxA(NULL, "ERROR-MOD!", NULL, NULL);
	//	exit(0);
	//}
	//Base = addr;
	//return true;
}

bool Memory::Read_Memory(ULONG64 Addr, void *Buffer, ULONG64 Size)
{
	/*RtlZeroMemory(_Hacker_Memory, sizeof(Kernel_RW_Memory_Struct));
	_Hacker_Memory->Size = Size;
	_Hacker_Memory->Status = 0;
	_Hacker_Memory->Addr = Addr;
	while (_Hacker_Memory->Addr != 0)
	{

	}
	RtlCopyMemory(Buffer, _Hacker_Memory->Buffer, Size);
	return true;*/


	return ReadProcessMemory(_Handle, (LPVOID)Addr, Buffer, Size, NULL);

	//return ReadMem(Addr, (char*)Buffer, Size);
}

bool Memory::Write_Memory(ULONG64 Addr, void *Buffer, ULONG64 Size)
{
	/*RtlZeroMemory(_Hacker_Memory, sizeof(Kernel_RW_Memory_Struct));
	RtlCopyMemory(_Hacker_Memory->Buffer, Buffer, Size);
	_Hacker_Memory->Size = Size;
	_Hacker_Memory->Status = 1;
	_Hacker_Memory->Addr = Addr;
	while (_Hacker_Memory->Addr != 0)
	{

	}
	return true;*/



	return WriteProcessMemory(_Handle, (LPVOID)Addr, &Buffer, Size, NULL);


	//return WriteMem(Addr, (char*)Buffer, Size);
}

void* Memory::Alloc_Memory(ULONG64 Size)
{
	/*RtlZeroMemory(_Hacker_Memory, sizeof(Kernel_RW_Memory_Struct));
	_Hacker_Memory->Status = 3;
	_Hacker_Memory->Size = Size;
	_Hacker_Memory->Addr = 1;
	while (_Hacker_Memory->Addr != 0)
	{

	}
	return (void*)_Hacker_Memory->Size;*/
	return false;
}

ULONG64 Memory::Get_Model_Addr()
{
	/*RtlZeroMemory(_Hacker_Memory, sizeof(Kernel_RW_Memory_Struct));
	_Hacker_Memory->Status = 4;
	RtlCopyMemory(_Hacker_Memory->Buffer, "RustClient.exe", strlen("RustClient.exe"));
	_Hacker_Memory->Addr = 1;
	while (_Hacker_Memory->Addr != 0)
	{

	}
	return _Hacker_Memory->Size;*/
	return Base;
}