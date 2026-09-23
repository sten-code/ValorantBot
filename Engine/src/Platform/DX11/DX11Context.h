#pragma once
#ifdef PLATFORM_WINDOWS

#include <Engine/Renderer/GraphicsContext.h>

#include <d3d11.h>

class DX11Context final : public GraphicsContext {
public:
    explicit DX11Context(const Window& window)
        : m_Window(window) {}
    ~DX11Context() override;

    void Initialize() override;
    void SwapBuffers() override;
    void Shutdown() override;
    void SetVSync(bool enabled) override { m_VSync = enabled; }

#ifdef ENGINE_IMGUI
    void InitializeImGui() override;
    void ImGuiBegin() override;
    void ImGuiEnd() override;
#endif

    void OnResize(uint32_t width, uint32_t height) override;

    // DirectX-specific methods
    [[nodiscard]] ID3D11Device* GetDevice() const { return m_Device; }
    [[nodiscard]] ID3D11RenderTargetView* GetRenderTargetView() const { return m_MainRenderTargetView; }

private:
    void CreateRenderTarget();
    void CleanupRenderTarget();
    void CaptureFrameIfRequested();

private:
    const Window& m_Window;

    ID3D11Device* m_Device = nullptr;
    ID3D11DeviceContext* m_DeviceContext = nullptr;
    IDXGISwapChain* m_SwapChain = nullptr;
    ID3D11RenderTargetView* m_MainRenderTargetView = nullptr;
    bool m_SwapChainOccluded = false;
    bool m_VSync = true;

#ifdef ENGINE_IMGUI
    bool m_ImGuiInitialized = false;
#endif
};

#endif