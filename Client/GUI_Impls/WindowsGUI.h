#pragma once

#include <stdexcept>

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_win32.h"
#include "imgui/examples/imgui_impl_dx11.h"
#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include <string>

#include "Windows.h"

#include "CrossPlatformGUI.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void EnterGUIMainLoop();
void DisplayErrorMessagebox(const std::string& message);

namespace WindowsGUIInternal
{
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}
