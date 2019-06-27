#include "DX_Show.h"

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

static int cycle = 0;
std::vector<std::string> WhiteList;
ULONG64 temptargetHEAD = 0;
void DX_Show::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//implement here
	if (cycle == 0)
	{
		//printf("blyatman\n");
		_Game_Data->Get_Data(); 
	}
	cycle++;
	if (cycle == 100)
	{
		cycle = 0;
		//printf("blyat\n");
	}

	float tempclosest = 100.f;
	std::string nawa;

	if (_Game_Data->_Player_Vector.size() < 1) // if less than 1 object
	{
		ImGui::GetOverlayDrawList()->AddText(m_pChinese, 15.f, ImVec2(0, 200),
			ImColor(1.0f, 1.0f, 1.0f, 1.0f), u8"不在游戏里");
	}
	else
	{
		ULONG64 copymem = _Game_Data->_Camera_Addr;
		views = Memory::_This->Read_Memory(copymem + ENTITYPTR_MATRIX, _Game_Data->_Test_MATRIX); // update camera
		for (int i = 0; i < _Game_Data->_Player_Vector.size(); i++)
		{
			Player ent = _Game_Data->_Player_Vector.at(i);
			if (ent.tag != 6 || ent.BaseObject ==  _Game_Data->localplayer.BaseObject)
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
				ImGui::GetOverlayDrawList()->AddText(m_pChinese, 15.f, ImVec2(position.x, position.y),
					ImColor(1.0f, 1.0f, 1.0f, 1.0f), VariableText("player %.2f", distance).c_str());

			}
			float dist = 0;
			dist = sqrtf(powf(GetSystemMetrics(SM_CXSCREEN) / 2 - head.x, 2.0) + powf(GetSystemMetrics(SM_CYSCREEN) / 2 - head.y, 2.0));
			if (dist < tempclosest && !GetAsyncKeyState(VK_XBUTTON2))
			{
				tempclosest = dist;
				temptargetHEAD = ent.head;
				nawa = ent.name;
			}
			if (head.x > 1 && head.y > 1)
				ImGui::GetOverlayDrawList()->AddCircle(ImVec2(head.x, head.y), 5, ImColor(255, 255, 0));
		}

		if (temptargetHEAD)
		{
			Vector3 temptarget = WorldToScreen(get_bone_pos(temptargetHEAD));
			float boneX = temptarget.x - GetSystemMetrics(SM_CXSCREEN) / 2;
			float boneY = temptarget.y - GetSystemMetrics(SM_CYSCREEN) / 2;

			if (_Loop == 2)
			{
				if (GetAsyncKeyState(VK_XBUTTON2) && temptarget.x != 0 && tempclosest != 0 && boneX != 0 && boneY != 0)
				{
					if (GetAsyncKeyState(VK_INSERT))
						WhiteList.push_back(nawa);
					AimBot(boneX, boneY);
				}
				_Loop = 0;
			}
			_Loop++;
		}

	}


EXIT:

	ImGui::Render();
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	ImVec4 clear_color = ImVec4(0, 0, 0, 0);
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	g_pSwapChain->Present(0, 0); // Present with vsync
}

