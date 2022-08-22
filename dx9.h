#pragma once
#include <d3d9.h>
#include "../D3DX/d3dx9.h"


#include <cstdint>
#include <utility>
#include <iostream>
#include "../lazy_importer.hpp"
#include "../skStr.h"
#include "../External/mouse_interface.h"


IDirect3D9Ex* p_Object = NULL;
IDirect3DDevice9Ex* p_Device = NULL;
D3DPRESENT_PARAMETERS p_Params = { NULL };

int Width;
int Height;



void CleanuoD3D()
{
	//hide_thread(LI_FN(GetCurrentThread).get()());
	if (p_Device != NULL)
	{
		p_Device->EndScene();
		p_Device->Release();
	}
	if (p_Object != NULL)
	{
		p_Object->Release();
	}
}


static HWND get_process_wnd(uint32_t pid) {
	std::pair<HWND, uint32_t> params = { 0, pid };

	BOOL bResult = LI_FN(EnumWindows).get()([](HWND hwnd, LPARAM lParam) -> BOOL {
		auto pParams = (std::pair<HWND, uint32_t>*)(lParam);
		uint32_t processId = 0;

		if (LI_FN(GetWindowThreadProcessId).get()(hwnd, reinterpret_cast<LPDWORD>(&processId)) && processId == pParams->second) {
			LI_FN(SetLastError).get()((uint32_t)-1);
			pParams->first = hwnd;
			return FALSE;
		}

		return TRUE;

		}, (LPARAM)&params);

	if (!bResult && LI_FN(GetLastError).get()() == -1 && params.first)
		return params.first;

	return NULL;
}

HWND hijacked_hwnd{};


namespace hijack {


	bool amd = false;
	bool nvidia = false;

	bool init() {
		//hide_thread(LI_FN(GetCurrentThread).get()());
		
		if (FindWindowA_Spoofed(skCrypt("CEF-OSC-WIDGET"), skCrypt("NVIDIA GeForce Overlay")))
		{
			nvidia = true;
			hijacked_hwnd = FindWindowA_Spoofed(skCrypt("CEF-OSC-WIDGET"), skCrypt("NVIDIA GeForce Overlay"));
		}
		else if (FindWindowA_Spoofed(skCrypt("AMDDVROVERLAYWINDOWCLASS"), skCrypt("amd dvr overlay")))
		{
			amd = true;
			hijacked_hwnd = FindWindowA_Spoofed(skCrypt("AMDDVROVERLAYWINDOWCLASS"), skCrypt("amd dvr overlay"));
		}
		else
		{
			std::cout << skCrypt("\n\t[+] NVIDIA/AMD not found!") << std::endl;
		}
		
		//hijacked_hwnd = FindWindowA_Spoofed(skCrypt("action_refresh_window_class"), NULL);
		if (!hijacked_hwnd) {
			//std::cout << "[-] failed to find target window handle" << std::endl;
			return false;
		}
	

		auto apply_window_styles = []() {
	
		
			SetWindowLongA_Spoofed(hijacked_hwnd, -20, static_cast<LONG_PTR>((static_cast<int>(GetWindowLongA_Spoofed(hijacked_hwnd, -20)) | 32))); //0x20
		
	        if (amd) MoveWindow_Spoofed(hijacked_hwnd, 0, 0, Width, Height, false); // only when amd

			// transparency
			MARGINS margin = { -2, -2, -2, -2 };
			DwmExtendFrameIntoClientArea_Spoofed(hijacked_hwnd, &margin);
			if (!SetLayeredWindowAttributes_Spoofed(hijacked_hwnd, 0, 255, 2)) { //0x000000 0xFF 0x02
			//	std::cout << "\t[-]" << " failed to set hijacked window attributes" << std::endl;
				return false;
			}

			SetWindowDisplayAffinity_Spoofed(hijacked_hwnd, WDA_EXCLUDEFROMCAPTURE);

			return true;
		};

		if (!apply_window_styles()) {
			return false;
		}


		return true;
	}
}

HRESULT DirectXInit(HWND hWnd)
{
	//hide_thread(LI_FN(GetCurrentThread).get()());
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		LI_FN(exit).get()(3);

	RtlSecureZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.hDeviceWindow = hWnd;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.BackBufferWidth = Width;
	p_Params.BackBufferHeight = Height;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;
	p_Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	//i put print here prints


	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
	{
		p_Object->Release();
		LI_FN(exit).get()(4);
	}
	//here doesnt



	p_Object->Release();
	return S_OK;
}
