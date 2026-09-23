#ifdef PLATFORM_WINDOWS
#include "Win32Window.h"
#include <Engine/Core/Log.h>
#include <Engine/Events/ApplicationEvents.h>
#include <Engine/Events/Event.h>
#include <Engine/Events/KeyEvents.h>
#include <Engine/Events/MouseEvents.h>

#include <dwmapi.h>
#include <windowsx.h>

#ifdef ENGINE_IMGUI
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

#ifdef IsMaximized
#undef IsMaximized
#endif

Win32Window::Win32Window(const WindowProps& props)
{
    Init(props);
}

Win32Window::~Win32Window()
{
    Shutdown();
}

#ifdef ENGINE_IMGUI
void Win32Window::InitializeImGui()
{
    ::ImGui_ImplWin32_Init(m_Hwnd);
    m_GraphicsContext->InitializeImGui();
    m_ImGuiInitialized = true;
}

void Win32Window::ImGuiBegin()
{
    m_GraphicsContext->ImGuiBegin();
    ::ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Win32Window::ImGuiEnd()
{
    ImGui::Render();
    m_GraphicsContext->ImGuiEnd();

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}
#endif

void Win32Window::Poll()
{
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

void Win32Window::Render()
{
    m_GraphicsContext->SwapBuffers();
}

void Win32Window::Shutdown()
{
    m_GraphicsContext->Shutdown();

#ifdef ENGINE_IMGUI
    if (m_ImGuiInitialized) {
        ::ImGui_ImplWin32_Shutdown();
        m_ImGuiInitialized = false;
    }
#endif

    if (m_Hwnd) {
        ::DestroyWindow(m_Hwnd);
        m_Hwnd = nullptr;
    }

    if (m_Instance) {
        ::UnregisterClass(m_Title.c_str(), m_Instance);
    }
}

void Win32Window::Restore() const
{
    ::ShowWindow(m_Hwnd, SW_RESTORE);
}

void Win32Window::Maximize() const
{
    ::ShowWindow(m_Hwnd, SW_MAXIMIZE);
}

void Win32Window::Minimize() const
{
    ::ShowWindow(m_Hwnd, SW_MINIMIZE);
}

void Win32Window::Close() const
{
    WindowCloseEvent event;
    m_EventCallback(event);
}

void Win32Window::SetVSync(bool enabled)
{
    m_GraphicsContext->SetVSync(enabled);
    m_VSync = enabled;
}

bool Win32Window::IsMaximized()
{
    return ::IsZoomed(m_Hwnd);
}

LRESULT Win32Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef ENGINE_IMGUI
    if (::ImGui_ImplWin32_WndProcHandler(m_Hwnd, uMsg, wParam, lParam)) {
        return true;
    }
#endif

    switch (uMsg) {
        case WM_ERASEBKGND: return TRUE;
        case WM_NCCALCSIZE: {
            if (m_Decorated) {
                break;
            }

            const int resizeBorderX = GetSystemMetrics(SM_CXFRAME);
            const int resizeBorderY = GetSystemMetrics(SM_CYFRAME);

            NCCALCSIZE_PARAMS* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
            RECT* requestedClientRect = params->rgrc;

            requestedClientRect->right -= resizeBorderX;
            requestedClientRect->left += resizeBorderX;
            requestedClientRect->bottom -= resizeBorderY;

            requestedClientRect->top += 0;

            return WVR_ALIGNTOP | WVR_ALIGNLEFT;
        }
        case WM_SIZE: {
            m_Width = LOWORD(lParam);
            m_Height = HIWORD(lParam);
            if (m_GraphicsContext) {
                m_GraphicsContext->OnResize(m_Width, m_Height);
            }

            if (m_EventCallback) {
                WindowResizeEvent event(m_Width, m_Height);
                m_EventCallback(event);
            }

            RECT sizeRect;
            GetWindowRect(m_Hwnd, &sizeRect);

            // Inform the application of the frame change to force redrawing with the new
            // client area that is extended into the title bar
            SetWindowPos(
                m_Hwnd, nullptr,
                sizeRect.left, sizeRect.top,
                sizeRect.right - sizeRect.left, sizeRect.bottom - sizeRect.top,
                SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
            return 0;
        }
        case WM_CLOSE: {
            if (m_EventCallback) {
                WindowCloseEvent event;
                m_EventCallback(event);
            }
            return 0;
        }
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: {
            if (m_EventCallback) {
                KeyPressedEvent event(static_cast<int>(wParam), (lParam & 0x40000000) ? 1 : 0);
                m_EventCallback(event);
            }
            return 0;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            if (m_EventCallback) {
                KeyReleasedEvent event(static_cast<int>(wParam));
                m_EventCallback(event);
            }
            return 0;
        }
        case WM_CHAR: {
            if (m_EventCallback) {
                KeyTypedEvent event(static_cast<unsigned int>(wParam));
                m_EventCallback(event);
            }
            return 0;
        }
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN: {
            int button = 0;
            switch (uMsg) {
                case WM_LBUTTONDOWN: button = 0; break;
                case WM_RBUTTONDOWN: button = 1; break;
                case WM_MBUTTONDOWN: button = 2; break;
                case WM_XBUTTONDOWN: button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; break;
                default: break;
            }
            if (m_EventCallback) {
                MouseButtonPressedEvent event(button);
                m_EventCallback(event);
            }
            return 0;
        }
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP: {
            int button = 0;
            switch (uMsg) {
                case WM_LBUTTONUP: button = 0; break;
                case WM_RBUTTONUP: button = 1; break;
                case WM_MBUTTONUP: button = 2; break;
                case WM_XBUTTONUP: button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; break;
                default: break;
            }
            if (m_EventCallback) {
                MouseButtonReleasedEvent event(button);
                m_EventCallback(event);
            }
            return 0;
        }
        case WM_MOUSEWHEEL: {
            if (m_EventCallback) {
                float yOffset = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
                MouseScrolledEvent event(0.0f, yOffset);
                m_EventCallback(event);
            }
            return 0;
        }
        case WM_MOUSEHWHEEL: {
            if (m_EventCallback) {
                float xOffset = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
                MouseScrolledEvent event(xOffset, 0.0f);
                m_EventCallback(event);
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (m_EventCallback) {
                int xPos = GET_X_LPARAM(lParam);
                int yPos = GET_Y_LPARAM(lParam);
                MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
                m_EventCallback(event);
            }
            return 0;
        }
        case WM_NCHITTEST: {
            if (m_Decorated) {
                break;
            }

            // Handle title bar hit testing
            if (m_IsTitleBarHovered) {
                // Return HTCAPTION to indicate the title bar is being hovered
                return HTCAPTION;
            }

            // Allow resizing from edges and dragging from client area
            LRESULT hit = DefWindowProc(m_Hwnd, uMsg, wParam, lParam);
            if (hit == HTCLIENT) {
                // Check if the cursor is in the resizable border
                POINT pt = { LOWORD(lParam), HIWORD(lParam) };
                ScreenToClient(m_Hwnd, &pt);
                RECT clientRect;
                GetClientRect(m_Hwnd, &clientRect);

                uint16_t x = pt.x, y = pt.y;

                // Define resizable border width
                int borderWidth = 5;

                // Check for left, right, top, bottom borders and corners
                if (x <= borderWidth) {
                    if (y <= borderWidth)
                        hit = HTTOPLEFT;
                    else if (y >= clientRect.bottom - borderWidth)
                        hit = HTBOTTOMLEFT;
                    else
                        hit = HTLEFT;
                } else if (x >= clientRect.right - borderWidth) {
                    if (y <= borderWidth)
                        hit = HTTOPRIGHT;
                    else if (y >= clientRect.bottom - borderWidth)
                        hit = HTBOTTOMRIGHT;
                    else
                        hit = HTRIGHT;
                } else if (y <= borderWidth) {
                    hit = HTTOP;
                } else if (y >= clientRect.bottom - borderWidth) {
                    hit = HTBOTTOM;
                }
            }
            return hit;
        }
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        default:
            break;
    }

    return ::DefWindowProc(m_Hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK Win32Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Win32Window* window = nullptr;

    if (uMsg == WM_CREATE) {
        const CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = static_cast<Win32Window*>(createStruct->lpCreateParams);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        window->m_Hwnd = hWnd; // Set the window handle after creation
    } else {
        window = reinterpret_cast<Win32Window*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (window) {
        return window->HandleMessage(uMsg, wParam, lParam);
    }

    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void Win32Window::Init(const WindowProps& props)
{
    m_Title = props.Title;
    m_Width = props.Width;
    m_Height = props.Height;
    m_Decorated = props.Decorated;

    INFO("Creating window {} ({}, {})", props.Title, props.Width, props.Height);

    m_Instance = ::GetModuleHandle(nullptr);
    RegisterWindowClass();
    CreateWindow(props);

    INFO("Creating graphics context");
    m_GraphicsContext = GraphicsContext::Create(*this);
    m_GraphicsContext->Initialize();

    SetVSync(false);
}

void Win32Window::RegisterWindowClass() const
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = m_Instance;
    wc.hIcon = ::LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = "CECLASS";
    wc.hIconSm = ::LoadIcon(nullptr, IDI_APPLICATION);

    if (!::RegisterClassEx(&wc)) {
        throw std::runtime_error("Failed to register window class");
    }
}

void Win32Window::CreateWindow(const WindowProps& props)
{
    m_Hwnd = ::CreateWindowEx(
        WS_EX_APPWINDOW,
        "CECLASS",
        m_Title.c_str(),
        WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME | WS_TABSTOP | WS_DLGFRAME | WS_BORDER,
        CW_USEDEFAULT, CW_USEDEFAULT,
        m_Width, m_Height,
        nullptr,
        nullptr,
        m_Instance,
        this);

    if (!m_Hwnd) {
        throw std::runtime_error("Failed to create window");
    }

    ::ShowWindow(m_Hwnd, SW_SHOWNA);
}

#endif
