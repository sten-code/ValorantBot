#include "window.h"
#include <iostream>

LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Window
{
    HWND g_Window;
    WNDCLASSEXW g_WindowClass;

    ID3D11Device* g_Device = nullptr;
    ID3D11DeviceContext* g_DeviceContext = nullptr;
    IDXGISwapChain* g_SwapChain = nullptr;
    ID3D11RenderTargetView* g_RenderTargetView = nullptr;

    bool g_Open = false;
    bool g_Enabled = false;
    WNDPROC g_OriginalWindowProcess = nullptr;
    HHOOK g_Hook;

    // Callback function for the keyboard hook
    LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam) 
    {
        if (nCode == HC_ACTION) 
        {
            // Check for keydown event
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) 
            {
                KBDLLHOOKSTRUCT* pKbdStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

                // Access information about the pressed key
                DWORD vkCode = pKbdStruct->vkCode;

                // toggle menu
                if (vkCode == VK_INSERT || (vkCode == VK_ESCAPE && Window::IsOpen()))
                {
                    Window::SetOpen(!Window::g_Open);
                }
            }
        }

        // Call the next hook in the hook chain
        return CallNextHookEx(g_Hook, nCode, wParam, lParam);
    }

    static constexpr ImVec4 FromRGB(int r, int g, int b)
    {
        return ImVec4{ r / 255.0f, g / 255.0f, b / 255.0f, 1.0f };
    }

    void SetThemeColors()
    {
        auto& style = ImGui::GetStyle();
        style.TabRounding = 4.0f;
        style.ChildRounding = 3.0f;
        style.FrameRounding = 3.0f;
        style.GrabRounding = 3.0f;
        style.WindowRounding = 3.0f;
        style.PopupRounding = 3.0f;

        // Header
        style.Colors[ImGuiCol_Header] = FromRGB(49, 50, 68);
        style.Colors[ImGuiCol_HeaderHovered] = FromRGB(69, 71, 90);
        style.Colors[ImGuiCol_HeaderActive] = FromRGB(49, 50, 68);

        // Buttons
        style.Colors[ImGuiCol_Button] = ImVec4{ 1, 1, 1, 0.07f };
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4{ 1, 1, 1, 0.1f };
        style.Colors[ImGuiCol_ButtonActive] = ImVec4{ 1, 1, 1, 0.08f };

        // Frame BG
        style.Colors[ImGuiCol_FrameBg] = FromRGB(49, 50, 68);
        style.Colors[ImGuiCol_FrameBgHovered] = FromRGB(69, 71, 90);
        style.Colors[ImGuiCol_FrameBgActive] = FromRGB(49, 50, 68);

        // Tabs
        style.Colors[ImGuiCol_Tab] = FromRGB(137, 180, 250);
        style.Colors[ImGuiCol_TabHovered] = FromRGB(137, 180, 250);
        style.Colors[ImGuiCol_TabActive] = FromRGB(243, 139, 168);
        style.Colors[ImGuiCol_TabUnfocused] = FromRGB(235, 160, 172);
        style.Colors[ImGuiCol_TabUnfocusedActive] = FromRGB(137, 180, 250);

        // Title
        style.Colors[ImGuiCol_TitleBg] = FromRGB(24, 24, 37);
        style.Colors[ImGuiCol_TitleBgActive] = FromRGB(30, 30, 46);
        style.Colors[ImGuiCol_TitleBgCollapsed] = FromRGB(127, 132, 156);
    }

    bool Setup(const std::string& windowName, const std::string& className)
    {
        g_Hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHook, GetModuleHandle(NULL), 0);

        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        g_WindowClass.cbSize = sizeof(WNDCLASSEXW);
        g_WindowClass.style = CS_HREDRAW | CS_VREDRAW;
        g_WindowClass.lpfnWndProc = DefWindowProc;
        g_WindowClass.hInstance = GetModuleHandle(NULL);
        g_WindowClass.lpszClassName = L"External Overlay Class";

        RegisterClassExW(&g_WindowClass);

        g_Window = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
            g_WindowClass.lpszClassName,
            L"External Overlay",
            WS_POPUP,
            0, 0,
            screenWidth, screenHeight,
            nullptr,
            nullptr,
            g_WindowClass.hInstance,
            nullptr
        );

        SetLayeredWindowAttributes(g_Window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

        g_OriginalWindowProcess = reinterpret_cast<WNDPROC>(
            SetWindowLongPtr(g_Window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc))
        );

        RECT clientArea = {};
        GetClientRect(g_Window, &clientArea);

        RECT windowArea = {};
        GetWindowRect(g_Window, &windowArea);

        POINT diff = {};
        ClientToScreen(g_Window, &diff);

        const MARGINS margins = {
            windowArea.left + (diff.x - windowArea.left),
            windowArea.top + (diff.y - windowArea.top),
            clientArea.right,
            clientArea.bottom
        };

        DwmExtendFrameIntoClientArea(g_Window, &margins);

        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferDesc.RefreshRate.Numerator = 60U;
        sd.BufferDesc.RefreshRate.Denominator = 1U;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1U;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 2U;
        sd.OutputWindow = g_Window;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        constexpr D3D_FEATURE_LEVEL levels[2]
        {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_0
        };

        D3D_FEATURE_LEVEL level = {};
        D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            0U,
            levels,
            2U,
            D3D11_SDK_VERSION,
            &sd,
            &g_SwapChain,
            &g_Device,
            &level,
            &g_DeviceContext
        );

        ID3D11Texture2D* backBuffer = nullptr;
        g_SwapChain->GetBuffer(0U, IID_PPV_ARGS(&backBuffer));

        if (backBuffer)
        {
            g_Device->CreateRenderTargetView(backBuffer, nullptr, &g_RenderTargetView);
            backBuffer->Release();
        }
        else
        {
            return false;
        }

        ShowWindow(g_Window, SW_SHOWDEFAULT);
        UpdateWindow(g_Window);

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();
        SetThemeColors();

        ImGui_ImplWin32_Init(g_Window);
        ImGui_ImplDX11_Init(g_Device, g_DeviceContext);

        return true;
    }

    bool Begin()
    {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                return false;
        }
        
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        return true;
    }

    void End()
    {
        ImGui::Render();

        constexpr float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        g_DeviceContext->OMSetRenderTargets(1U, &g_RenderTargetView, nullptr);
        if (g_RenderTargetView)
            g_DeviceContext->ClearRenderTargetView(g_RenderTargetView, color);

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_SwapChain->Present(1U, 0U);
    }

    void Shutdown()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();

        ImGui::DestroyContext();

        if (g_SwapChain)
            g_SwapChain->Release();

        if (g_DeviceContext)
            g_DeviceContext->Release();

        if (g_Device)
            g_Device->Release();

        if (g_RenderTargetView)
            g_RenderTargetView->Release();

        DestroyWindow(g_Window);
        UnregisterClassW(g_WindowClass.lpszClassName, g_WindowClass.hInstance);
    
        UnhookWindowsHookEx(g_Hook);
    }

    bool IsOpen()
    {
        return g_Open;
    }

    void SetOpen(bool open)
    {
        if (g_Open == open || !g_Enabled)
            return;

        g_Open = open;

        DWORD newStyle = GetWindowLongPtr(Window::g_Window, GWL_EXSTYLE);
        if (g_Open)
            newStyle &= ~WS_EX_LAYERED;
        else
            newStyle |= WS_EX_LAYERED;
        SetWindowLongPtr(Window::g_Window, GWL_EXSTYLE, newStyle);

        if (g_Open)
            SetForegroundWindow(Window::g_Window);
    }

    bool IsEnabled()
    {
        return g_Enabled;
    }

    void SetEnabled(bool enabled)
    {
        g_Enabled = enabled;

        if (!g_Open)
            return;

        DWORD newStyle = GetWindowLongPtr(Window::g_Window, GWL_EXSTYLE);
        if (enabled)
            newStyle &= ~WS_EX_LAYERED;
        else
            newStyle |= WS_EX_LAYERED;
        SetWindowLongPtr(Window::g_Window, GWL_EXSTYLE, newStyle);
    }

    HWND GetWindow()
    {
        return g_Window;
    }
}

LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (Window::g_Open && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return 1L;

    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0L;
    default:
        break;
    }

    return ::CallWindowProc(Window::g_OriginalWindowProcess, hWnd, msg, wParam, lParam);
}