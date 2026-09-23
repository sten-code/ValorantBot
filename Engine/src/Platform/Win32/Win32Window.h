#pragma once
#ifdef PLATFORM_WINDOWS

#include <Engine/Core/Window.h>

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <string>

#ifdef CreateWindow
#undef CreateWindow
#endif

class Win32Window final : public Window {
public:
    explicit Win32Window(const WindowProps& props);
    ~Win32Window() override;

#ifdef ENGINE_IMGUI
    void InitializeImGui() override;
    void ImGuiBegin() override;
    void ImGuiEnd() override;
#endif

    void Poll() override;
    void Render() override;
    void Shutdown() override;

    void Restore() const override;
    void Maximize() const override;
    void Minimize() const override;
    void Close() const override;

    [[nodiscard]] void* GetNativeHandle() const override { return static_cast<void*>(m_Hwnd); }

    [[nodiscard]] uint32_t GetWidth() const override { return m_Width; }
    [[nodiscard]] uint32_t GetHeight() const override { return m_Height; }

    [[nodiscard]] bool IsVSync() const override { return m_VSync; }
    void SetVSync(bool enabled) override;

    bool IsMaximized() override;

    /// <summary>
    /// This is used by GLFW to check whether the title bar is being hovered.
    /// This function is intended to be called every frame, so GLFW knows the hitbox of the title bar.
    /// </summary>
    /// <param name="hovered"></param>
    void SetIsTitleBarHovered(bool hovered) override { m_IsTitleBarHovered = hovered; };

    void SetEventCallback(const EventCallbackFn& callback) override { m_EventCallback = callback; }

    GraphicsContext& GetGraphicsContext() override { return *m_GraphicsContext; }

private:
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void Init(const WindowProps& props);

    void RegisterWindowClass() const;
    void CreateWindow(const WindowProps& props);
    void ProcessEvents();

private:
    HWND m_Hwnd = nullptr;
    HINSTANCE m_Instance = nullptr;
    EventCallbackFn m_EventCallback = nullptr;
    std::unique_ptr<GraphicsContext> m_GraphicsContext = nullptr;

    bool m_IsTitleBarHovered = false;

    std::string m_Title;
    uint32_t m_Width = 0, m_Height = 0;
    bool m_VSync = false;
    bool m_Decorated = true;

#ifdef ENGINE_IMGUI
    bool m_ImGuiInitialized = false;
#endif

    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif