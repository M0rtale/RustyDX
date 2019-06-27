#pragma once

#include <windows.h>
#include <TlHelp32.h>
#include <iostream>


struct Kernel_RW_Memory_Struct
{
	ULONG_PTR Addr;
	ULONG_PTR Size;
	char Buffer[1024];
	ULONG Status;
	Kernel_RW_Memory_Struct()
	{
		RtlZeroMemory(Buffer, 1024);
		Addr = 0;
		Size = 0;
		Status = 0;
	}
};


class Memory
{
public:
	Memory()
	{
		_This = this;
	}
	~Memory() = default;
public:
	bool Init();
	bool Read_Memory(ULONG64 Addr, void *Buffer, ULONG64 Size);
	bool Write_Memory(ULONG64 Addr, void *Buffer, ULONG64 Size);
	void* Alloc_Memory(ULONG64 Size);
	ULONG64 Get_Model_Addr();
	ULONG64 Base = 0;
public:
	static Memory *_This;
public:
	template<typename T>
	T Read_Memory(ULONG64 Addr, T t)
	{
		/*RtlZeroMemory(_Hacker_Memory, sizeof(Kernel_RW_Memory_Struct));
		_Hacker_Memory->Size = sizeof(T);
		_Hacker_Memory->Status = 0;
		_Hacker_Memory->Addr = Addr;
		while (_Hacker_Memory->Addr != 0)
		{

		}
		T t2;
		RtlCopyMemory(&t2, _Hacker_Memory->Buffer, _Hacker_Memory->Size);
		return t2;*/
		T ret;
		ReadProcessMemory(_Handle, (LPVOID)Addr, &ret, sizeof(T), NULL);
		//ReadMem(Addr, &ret, sizeof(T));
		return ret;
	}

	template<typename T>
	T Read_Memory(ULONG64 Addr)
	{
		/*RtlZeroMemory(_Hacker_Memory, sizeof(Kernel_RW_Memory_Struct));
		_Hacker_Memory->Size = sizeof(T);
		_Hacker_Memory->Status = 0;
		_Hacker_Memory->Addr = Addr;
		while (_Hacker_Memory->Addr != 0)
		{

		}
		T t2;
		RtlCopyMemory(&t2, _Hacker_Memory->Buffer, _Hacker_Memory->Size);
		return t2;*/
		T ret;
		ReadProcessMemory(_Handle, (LPVOID)Addr, &ret, sizeof(T), NULL);
		//ReadMem(Addr, &ret, sizeof(T));
		return ret;
	}
private:
	HANDLE _Handle;
	Kernel_RW_Memory_Struct *_Hacker_Memory = nullptr;

};

