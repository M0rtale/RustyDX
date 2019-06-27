// Entry Point

#include "OverlayWindow.h"

OverlayWindow *_Overlay = nullptr;

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

int main()
{
	getchar();
	printf(XorStr("BOIHOOK Deploying\n"));

	_Overlay = new OverlayWindow();
	_Overlay->Init();

	MSG msg;
	GetMessage(&msg, nullptr, 0, 0);
	printf(XorStr("BOIHOOK Deployed\n"));
	printf(XorStr("Don't act like a fucking dumbass\n"));
	printf(XorStr("Enjoy your party\n"));
	printf(XorStr("-------------------------------\n"));
	printf(XorStr("\nF4 - AIM\n"));
	printf(XorStr("\n-------------------------------"));

	//ULONG64 character = 0;

	//ULONG64 base = 0;
	//ULONG64 shit = _Overlay->_DX_Show->_Game_Data->_Memory->Get_Model_Addr();
	//printf("Image is: %llu \n", shit);

	//base = _Overlay->_DX_Show->_Game_Data->_Memory->Read_Memory(shit, character);
	//printf("Base is: %llu \n", base);

	//base = _Overlay->_DX_Show->_Game_Data->_Memory->Read_Memory(shit + RUST_OBJECTMANAGER, character);
	//printf("Manager is: %llu \n", base);

	//Sleep(5000);

	//base = _Overlay->_DX_Show->_Game_Data->_Memory->Read_Memory(shit + RUST_OBJECTMANAGER, character);
	//printf("Manager is: %llu \n", base);

	//base = _Overlay->_DX_Show->_Game_Data->_Memory->Read_Memory(shit, character);
	//printf("Base is: %llu \n", base);

	//getchar();
	//return 0;

	while (msg.message != WM_QUIT)
	{

		SetWindowPos(_Overlay->_Hwnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{

			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		_Overlay->_DX_Show->Render();

		if (GetPIDForProcess(XorStr("RustClient.exe")) == 0)
			break;

		if (!FindWindowA("Rusty_Rust", NULL)) // 如果透明窗口被关闭了
			break;
	}
}