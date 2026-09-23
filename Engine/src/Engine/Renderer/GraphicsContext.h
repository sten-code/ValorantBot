#pragma once

#include <cstdint>
#include <memory>

class Window;

class GraphicsContext {
public:
    virtual ~GraphicsContext() = default;

    virtual void Initialize() = 0;
    virtual void SwapBuffers() = 0;
    virtual void Shutdown() = 0;
    virtual void SetVSync(bool enabled) = 0;

#ifdef ENGINE_IMGUI
    virtual void InitializeImGui() = 0;
    virtual void ImGuiBegin() = 0;
    virtual void ImGuiEnd() = 0;
#endif

    virtual void OnResize(uint32_t width, uint32_t height) = 0;

    static std::unique_ptr<GraphicsContext> Create(const Window& window);
};
