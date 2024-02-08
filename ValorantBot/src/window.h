#pragma once

#include <d3d11.h>
#include <dwmapi.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include <string>

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Window
{
	bool Setup(const std::string& windowName, const std::string& className);
	bool Begin();
	void End();
	void Shutdown();
	bool IsOpen();
	void SetOpen(bool open);
	bool IsEnabled();
	void SetEnabled(bool enabled);
	HWND GetWindow();
}