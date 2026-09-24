#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstddef>

#include <d3d11.h>
#include <dxgi1_2.h>

#include <winrt/base.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>

class ScreenCapture {
public:
    ScreenCapture(int width, int height);
    ~ScreenCapture();

    void Resize(int width, int height);

    uint8_t* CaptureScreen(int x, int y);

    [[nodiscard]] size_t GetBufferSize() const { return m_BufferSize; }
    [[nodiscard]] int GetWidth() const { return m_Width; }
    [[nodiscard]] int GetHeight() const { return m_Height; }

private:
    bool InitializeD3D();
    bool InitializeCapture(int x, int y);
    bool CreateStagingTexture(int width, int height);

    void ReleaseCapture();

private:
    int m_Width = 0;
    int m_Height = 0;

    uint8_t* m_Buffer = nullptr;
    size_t m_BufferSize = 0;

    // D3D11
    winrt::com_ptr<ID3D11Device> m_Device;
    winrt::com_ptr<ID3D11DeviceContext> m_Context;

    winrt::com_ptr<ID3D11Texture2D> m_StagingTexture;

    // WGC
    winrt::Windows::Graphics::Capture::GraphicsCaptureItem
        m_CaptureItem{ nullptr };

    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool
        m_FramePool{ nullptr };

    winrt::Windows::Graphics::Capture::GraphicsCaptureSession
        m_CaptureSession{ nullptr };

    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice
        m_WinRTDevice{ nullptr };

    // Monitor currently being captured.
    HMONITOR m_Monitor = nullptr;

    bool m_Initialized = false;
};