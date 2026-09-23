#pragma once

#include <Engine/Events/Event.h>
#include <Engine/Renderer/GraphicsContext.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

struct WindowProps {
    std::string Title = "ImGui Application";
    uint32_t Width = 1280;
    uint32_t Height = 720;
    bool Decorated = true;
};

class Window {
public:
    using EventCallbackFn = std::function<void(Event&)>;

    virtual ~Window() = default;

#ifdef ENGINE_IMGUI
    virtual void InitializeImGui() = 0;
    virtual void ImGuiBegin() = 0;
    virtual void ImGuiEnd() = 0;
#endif

    virtual void Poll() = 0;
    virtual void Render() = 0;
    virtual void Shutdown() = 0;

    virtual void Restore() const = 0;
    virtual void Maximize() const = 0;
    virtual void Minimize() const = 0;
    virtual void Close() const = 0;

    [[nodiscard]] virtual void* GetNativeHandle() const = 0;

    [[nodiscard]] virtual uint32_t GetWidth() const = 0;
    [[nodiscard]] virtual uint32_t GetHeight() const = 0;

    [[nodiscard]] virtual bool IsVSync() const = 0;
    virtual void SetVSync(bool enabled) = 0;

    [[nodiscard]] virtual bool IsMaximized() = 0;
    virtual void SetIsTitleBarHovered(bool hovered) = 0;
    virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

    virtual GraphicsContext& GetGraphicsContext() = 0;

    static std::unique_ptr<Window> Create(const WindowProps& props = WindowProps());
};