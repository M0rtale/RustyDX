#include "DX_Show.h"
#include<stdio.h>

bool DX_Show::Init(HWND hwnd)
{
	if (CreateDeviceD3D(hwnd) < 0)
	{
		CleanupDeviceD3D();
		//MessageBox(NULL, L"jawlkfawj", L"fjawoifjawoifj", 0);
		return false;
	}

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = 0;
	style.WindowRounding = 0;
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 0);
	style.Colors[ImGuiCol_Border] = ImVec4(255, 0, 0, 255);

	_Game_Data->Hook_Start();
}

void DX_Show::Cleanup()
{
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

std::string GetFileVersion(const char* filename)
{
	DWORD  verHandle = 0;
	UINT   size = 0;
	LPBYTE lpBuffer = NULL;
	DWORD  verSize = GetFileVersionInfoSize(filename, &verHandle);

	if (verSize != NULL)
	{
		LPSTR verData = new char[verSize];

		if (GetFileVersionInfo(filename, verHandle, verSize, verData))
		{
			if (VerQueryValue(verData, "\\", (VOID FAR * FAR*) & lpBuffer, &size))
			{
				if (size)
				{
					VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
					if (verInfo->dwSignature == 0xfeef04bd)
					{

						// Doesn't matter if you are on 32 bit or 64 bit,
						// DWORD is always 32 bits, so first two revision numbers
						// come from dwFileVersionMS, last two come from dwFileVersionLS
						return VariableText("%d.%d.%d.%d\n",
							(verInfo->dwFileVersionMS >> 16) & 0xffff,
							(verInfo->dwFileVersionMS >> 0) & 0xffff,
							(verInfo->dwFileVersionLS >> 16) & 0xffff,
							(verInfo->dwFileVersionLS >> 0) & 0xffff
						);
					}
				}
			}
		}
		delete[] verData;
	}
	return "";
}

static int cycle = 0;
std::vector<std::string> WhiteList;
ULONG64 temptargetHEAD = 0;
static int crosshair = 1;
static bool startThread = false;
void DX_Show::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	cycle++;
	if (cycle == 1000)
	{
		cycle = 0;
		_Game_Data->Get_Data();
		//printf("blyat\n");
	}

	float tempclosest = 100.f;
	std::string nawa;

	if (_Game_Data->_Player_Vector.size() < 1) // if less than 1 object
	{
		TCHAR szExeFileName[MAX_PATH];
		GetModuleFileName(NULL, szExeFileName, MAX_PATH);

		ImGui::GetOverlayDrawList()->AddText(m_pChinese, 15.f, ImVec2(0, 200),
			ImColor(1.0f, 1.0f, 1.0f, 1.0f), VariableText("Not In game... Current Version %s", GetFileVersion(szExeFileName)).c_str());
	}
	else
	{
		TCHAR szExeFileName[MAX_PATH];
		GetModuleFileName(NULL, szExeFileName, MAX_PATH);


		ImGui::GetOverlayDrawList()->AddText(m_pChinese, 15.f, ImVec2(86, 86),
			ImColor(.5f, 1.0f, 1.0f, 1.0f), XorStr("Welcome back Allen:)\nF3 - name\nF2 - crosshair\nz or mouse5 - aimbot\nIns - Whitelist\nDel - clear whitelist"));

		ImGui::GetOverlayDrawList()->AddText(m_pChinese, 15.f, ImVec2(GetSystemMetrics(SM_CXSCREEN) / 2 - 9, GetSystemMetrics(SM_CYSCREEN) - 15),
			ImColor(.5f, 1.0f, 1.0f, 1.0f), VariableText(u8"%s", GetFileVersion(szExeFileName)).c_str());

		ULONG64 copymem = _Game_Data->_Camera_Addr;
		views = Memory::_This->Read_Memory(copymem + ENTITYPTR_MATRIX, _Game_Data->_Test_MATRIX); // update camera
		for (int i = 0; i < _Game_Data->_Player_Vector.size(); i++)
		{
			Player ent = _Game_Data->_Player_Vector.at(i);
			if (ent.tag != 6 || ent.BaseObject ==  _Game_Data->localplayer.BaseObject || ent.flag & PlayerFlags::Sleeping) // todo: fix this
			{
				continue;
			}
			if (WhiteList.end() != std::find(WhiteList.begin(), WhiteList.end(), ent.name)) // if whitelisted.
				continue;
			ent.position = _Game_Data->_Memory->Read_Memory(ent.VisualState + VISUALSTATE_POSITION, _Game_Data->_Test_Vector3);
			_Game_Data->localplayer.position = _Game_Data->_Memory->Read_Memory(_Game_Data->localplayer.VisualState + VISUALSTATE_POSITION, _Game_Data->_Test_Vector3);
			Vector3 position = WorldToScreen(ent.position);
			Vector3 head = WorldToScreen(get_bone_pos(ent.head));
			float distance = _Game_Data->localplayer.position.Distance(ent.position);
			if (distance >= 325.f)
				continue;
			if (position.x > 1 && position.y > 1)
			{
				if (!GetAsyncKeyState(VK_F3))
					ImGui::GetOverlayDrawList()->AddText(m_pChinese, 15.f, ImVec2(position.x, position.y),
						ImColor(1.0f, 1.0f, 1.0f, 1.0f), VariableText("%s %.2f", ent.objName, distance).c_str());
				else
					ImGui::GetOverlayDrawList()->AddText(m_pChinese, 15.f, ImVec2(position.x, position.y),
						ImColor(1.0f, 1.0f, 1.0f, 1.0f), VariableText("%s %.2f", ent.name, distance).c_str());

			}
			float dist = 0;
			dist = sqrtf(powf(GetSystemMetrics(SM_CXSCREEN) / 2 - head.x, 2.0) + powf(GetSystemMetrics(SM_CYSCREEN) / 2 - head.y, 2.0));
			if (dist < tempclosest && (!GetAsyncKeyState(VK_XBUTTON2) || !GetAsyncKeyState(90)))
			{
				tempclosest = dist;
				temptargetHEAD = ent.head;
				nawa = ent.name;
			}
			if (head.x > 1 && head.y > 1)
				ImGui::GetOverlayDrawList()->AddCircle(ImVec2(head.x, head.y), 5, ImColor(255, 255, 0));
		}
		if (false && GetAsyncKeyState(VK_F3)) // TODOL Fix Active List or implement networked list
		{
			for (int i = 0; i < _Game_Data->_Obj_Vector.size(); i++)
			{
				Player ent = _Game_Data->_Obj_Vector.at(i);
				ent.position = _Game_Data->_Memory->Read_Memory(ent.VisualState + VISUALSTATE_POSITION, _Game_Data->_Test_Vector3);
				float distance = _Game_Data->localplayer.position.Distance(ent.position);
				//if (distance >= 325.f)
					//continue;
				Vector3 pos = WorldToScreen(ent.position);
				if (strstr(ent.name.c_str(), "metal"))
					ImGui::GetOverlayDrawList()->AddText(m_pChinese, 12.f, ImVec2(pos.x, pos.y), ImColor(255, 0, 255), VariableText("Metal %.2f", distance).c_str());
				if (strstr(ent.name.c_str(), "stone"))
					ImGui::GetOverlayDrawList()->AddText(m_pChinese, 12.f, ImVec2(pos.x, pos.y), ImColor(255, 255, 255), VariableText("stone %.2f", distance).c_str());
				if (strstr(ent.name.c_str(), "tree"))
					ImGui::GetOverlayDrawList()->AddText(m_pChinese, 12.f, ImVec2(pos.x, pos.y), ImColor(165, 42, 42), VariableText("tree %.2f", distance).c_str());
				if (strstr(ent.name.c_str(), "supply_drop"))
					ImGui::GetOverlayDrawList()->AddText(m_pChinese, 12.f, ImVec2(pos.x, pos.y), ImColor(0,0,255), VariableText("supply_drop %.2f", distance).c_str());
				if (strstr(ent.name.c_str(), "cupboard"))
					ImGui::GetOverlayDrawList()->AddText(m_pChinese, 12.f, ImVec2(pos.x, pos.y), ImColor(165, 42, 42), VariableText("cupboard %.2f", distance).c_str());
				if (strstr(ent.name.c_str(), "stash"))
					ImGui::GetOverlayDrawList()->AddText(m_pChinese, 12.f, ImVec2(pos.x, pos.y), ImColor(165, 42, 42), VariableText("stash %.2f", distance).c_str());
				if (strstr(ent.name.c_str(), "trash-pile"))
					ImGui::GetOverlayDrawList()->AddText(m_pChinese, 12.f, ImVec2(pos.x, pos.y), ImColor(128, 128, 128), VariableText("trash %.2f", distance).c_str());
				if (strstr(ent.name.c_str(), "loot_barrel"))
					ImGui::GetOverlayDrawList()->AddText(m_pChinese, 12.f, ImVec2(pos.x, pos.y), ImColor(255,0,0), VariableText("barrel %.2f", distance).c_str());
				if (strstr(ent.name.c_str(), "hemp"))
					ImGui::GetOverlayDrawList()->AddText(m_pChinese, 12.f, ImVec2(pos.x, pos.y), ImColor(0, 255, 0), VariableText("hemp %.2f", distance).c_str());
				if (strstr(ent.name.c_str(), "crate"))
					ImGui::GetOverlayDrawList()->AddText(m_pChinese, 12.f, ImVec2(pos.x, pos.y), ImColor(165, 42, 42), VariableText("crate %.2f", distance).c_str());
				if (strstr(ent.name.c_str(), "pumpkin"))
					ImGui::GetOverlayDrawList()->AddText(m_pChinese, 12.f, ImVec2(pos.x, pos.y), ImColor(255,118,25), VariableText("pumpkin %.2f", distance).c_str());
				if (strstr(ent.name.c_str(), "corn"))
					ImGui::GetOverlayDrawList()->AddText(m_pChinese, 12.f, ImVec2(pos.x, pos.y), ImColor(255, 255, 0), VariableText("corn %.2f", distance).c_str());
				if (strstr(ent.name.c_str(), "woodbox"))
					ImGui::GetOverlayDrawList()->AddText(m_pChinese, 12.f, ImVec2(pos.x, pos.y), ImColor(165, 42, 42), VariableText("woodbox %.2f", distance).c_str());
				if (strstr(ent.name.c_str(), "box.wooden"))
					ImGui::GetOverlayDrawList()->AddText(m_pChinese, 12.f, ImVec2(pos.x, pos.y), ImColor(165, 42, 42), VariableText("box.wooden %.2f", distance).c_str());
			}
		}
		 
		if (GetAsyncKeyState(VK_INSERT) && nawa != "")
			WhiteList.push_back(nawa);
		if (GetAsyncKeyState(VK_DELETE))
			WhiteList.clear(); 

		if (temptargetHEAD)
		{
			Vector3 temptarget = WorldToScreen(get_bone_pos(temptargetHEAD));
			float boneX = temptarget.x - GetSystemMetrics(SM_CXSCREEN) / 2;
			float boneY = temptarget.y - GetSystemMetrics(SM_CYSCREEN) / 2;

			if (_Loop == 2)
			{
				if ((GetAsyncKeyState(VK_XBUTTON2) || GetAsyncKeyState(90)) && temptarget.x != 0 && tempclosest != 0 && boneX != 0 && boneY != 0)
				{
					AimBot(boneX, boneY);
				}
				_Loop = 0;
			}
			_Loop++;
		}

	}

	//draw crosshair
	int drX = GetSystemMetrics(SM_CXSCREEN) / 2;
	int drY = GetSystemMetrics(SM_CYSCREEN) / 2;
	if (crosshair == 1)
	{
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(drX - 5, drY), ImVec2(drX + 5, drY), ImColor(255, 255, 255));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(drX, drY - 5), ImVec2(drX, drY + 5), ImColor(255, 255, 255));
	}
	if (crosshair == 2)
	{
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(drX, drY), ImVec2(drX, drY - 5), ImColor(255, 255, 255));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(drX, drY - 5), ImVec2(drX + 5, drY - 5), ImColor(255, 255, 255));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(drX, drY), ImVec2(drX + 5, drY), ImColor(255, 255, 255));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(drX + 5, drY), ImVec2(drX + 5, drY + 5), ImColor(255, 255, 255));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(drX, drY), ImVec2(drX, drY + 5), ImColor(255, 255, 255));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(drX, drY + 5), ImVec2(drX - 5, drY + 5), ImColor(255, 255, 255));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(drX, drY), ImVec2(drX - 5, drY), ImColor(255, 255, 255));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(drX - 5, drY), ImVec2(drX - 5, drY - 5), ImColor(255, 255, 255));
	}

	int y = 10;
	for (std::string p : WhiteList)
	{
		ImGui::GetOverlayDrawList()->AddText(ImVec2(10, y), ImColor(255, 255, 0), p.c_str());
		y += 20;
	}

	if (GetAsyncKeyState(VK_F2))
	{
		crosshair++;
		if (crosshair == 3)
			crosshair = 1;
	}

EXIT:

	ImGui::Render();
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	ImVec4 clear_color = ImVec4(0, 0, 0, 0);
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	g_pSwapChain->Present(0, 0); // Present with vsync
}

