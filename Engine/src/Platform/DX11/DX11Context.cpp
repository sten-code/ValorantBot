#ifdef PLATFORM_WINDOWS
#include "DX11Context.h"
#include <Engine/Core/Log.h>
#include <Engine/Core/Window.h>

#include <cstdio>
#include <vector>

#ifdef ENGINE_IMGUI
#include <imgui_impl_DX11.h>
#endif

DX11Context::~DX11Context()
{
    Shutdown();
}

void DX11Context::Initialize()
{
    HWND hWnd = static_cast<HWND>(m_Window.GetNativeHandle());

    INFO("Creating swapchain");

    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    constexpr UINT createDeviceFlags = 0;
    // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    constexpr D3D_FEATURE_LEVEL featureLevelArray[2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };
    HRESULT res = ::D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
        featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &m_SwapChain,
        &m_Device, &featureLevel, &m_DeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = ::D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags,
            featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &m_SwapChain, &m_Device,
            &featureLevel, &m_DeviceContext);

    ASSERT(res == S_OK, "Failed to create DirectX 10 device and swap chain");

    CreateRenderTarget();
}

void DX11Context::SwapBuffers()
{
    CaptureFrameIfRequested();

    // Present with vsync (1) and handle window occlusion
    HRESULT result = m_SwapChain->Present(m_VSync, 0);

    // Check if the window is occluded (minimized, etc.)
    m_SwapChainOccluded = (result == DXGI_STATUS_OCCLUDED);
}

void DX11Context::CaptureFrameIfRequested()
{
    FILE* flag = nullptr;
    if (fopen_s(&flag, "capture.flag", "rb") != 0) {
        return;
    }
    if (!flag) {
        return;
    }
    fclose(flag);
    remove("capture.flag");

    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)))) {
        return;
    }

    D3D11_TEXTURE2D_DESC desc;
    backBuffer->GetDesc(&desc);

    D3D11_TEXTURE2D_DESC stageDesc = desc;
    stageDesc.BindFlags = 0;
    stageDesc.MiscFlags = 0;
    stageDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    stageDesc.Usage = D3D11_USAGE_STAGING;

    ID3D11Texture2D* stage = nullptr;
    if (FAILED(m_Device->CreateTexture2D(&stageDesc, nullptr, &stage))) {
        backBuffer->Release();
        return;
    }

    m_DeviceContext->CopyResource(stage, backBuffer);

    D3D11_MAPPED_SUBRESOURCE mapped;
    if (SUCCEEDED(m_DeviceContext->Map(stage, 0, D3D11_MAP_READ, 0, &mapped))) {
        const uint32_t w = desc.Width;
        const uint32_t h = desc.Height;
        const uint32_t stride = w * 3;
        const uint32_t pad = (4 - (stride % 4)) % 4;
        const uint32_t paddedStride = stride + pad;
        const uint32_t dataSize = paddedStride * h;
        const uint32_t fileSize = 54 + dataSize;

        FILE* out = nullptr;
        if (fopen_s(&out, "capture.bmp", "wb") == 0 && out) {
            const uint8_t header[54] = {
                'B', 'M',
                static_cast<uint8_t>(fileSize), static_cast<uint8_t>(fileSize >> 8),
                static_cast<uint8_t>(fileSize >> 16), static_cast<uint8_t>(fileSize >> 24),
                0, 0, 0, 0,
                54, 0, 0, 0,
                40, 0, 0, 0,
                static_cast<uint8_t>(w), static_cast<uint8_t>(w >> 8),
                static_cast<uint8_t>(w >> 16), static_cast<uint8_t>(w >> 24),
                static_cast<uint8_t>(h), static_cast<uint8_t>(h >> 8),
                static_cast<uint8_t>(h >> 16), static_cast<uint8_t>(h >> 24),
                1, 0, 24, 0, 0, 0, 0, 0,
                static_cast<uint8_t>(dataSize), static_cast<uint8_t>(dataSize >> 8),
                static_cast<uint8_t>(dataSize >> 16), static_cast<uint8_t>(dataSize >> 24),
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
            };
            fwrite(header, 1, sizeof(header), out);

            const uint8_t* src = static_cast<const uint8_t*>(mapped.pData);
            std::vector<uint8_t> row(paddedStride);
            for (uint32_t y = 0; y < h; ++y) {
                const uint8_t* s = src + (h - 1 - y) * static_cast<size_t>(mapped.RowPitch);
                for (uint32_t x = 0; x < w; ++x) {
                    row[x * 3 + 0] = s[x * 4 + 2]; // B
                    row[x * 3 + 1] = s[x * 4 + 1]; // G
                    row[x * 3 + 2] = s[x * 4 + 0]; // R
                }
                fwrite(row.data(), 1, paddedStride, out);
            }
            fclose(out);
            INFO("Captured capture.bmp ({}x{})", w, h);
        }
        m_DeviceContext->Unmap(stage, 0);
    }

    stage->Release();
    backBuffer->Release();
}

void DX11Context::Shutdown()
{
#ifdef ENGINE_IMGUI
    if (m_ImGuiInitialized) {
        ::ImGui_ImplDX11_Shutdown();
        m_ImGuiInitialized = false;
    }
#endif

    CleanupRenderTarget();
    if (m_SwapChain) {
        m_SwapChain->Release();
        m_SwapChain = nullptr;
    }
    if (m_Device) {
        m_Device->Release();
        m_Device = nullptr;
    }
}

#ifdef ENGINE_IMGUI
void DX11Context::ImGuiBegin()
{
    ::ImGui_ImplDX11_NewFrame();
}

void DX11Context::ImGuiEnd()
{
    constexpr float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_DeviceContext->OMSetRenderTargets(1, &m_MainRenderTargetView, nullptr);
    m_DeviceContext->ClearRenderTargetView(m_MainRenderTargetView, clearColor);
    ::ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void DX11Context::InitializeImGui()
{
    ::ImGui_ImplDX11_Init(m_Device, m_DeviceContext);
    m_ImGuiInitialized = true;
}
#endif

void DX11Context::OnResize(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0 || !m_Device || !m_SwapChain) {
        return;
    }

    CleanupRenderTarget();

    // Preserve the existing swap chain format
    HRESULT res = m_SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    ASSERT(SUCCEEDED(res), "Failed to resize DX11 swapchain buffers");

    CreateRenderTarget();
}

void DX11Context::CreateRenderTarget()
{
    // INFO("Creating render target");
    ID3D11Texture2D* backBuffer = nullptr;
    if (SUCCEEDED(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)))) {
        HRESULT res = m_Device->CreateRenderTargetView(backBuffer, nullptr, &m_MainRenderTargetView);
        ASSERT(SUCCEEDED(res), "Failed to create render target view");
        backBuffer->Release();
    }
}

void DX11Context::CleanupRenderTarget()
{
    if (m_MainRenderTargetView) {
        m_MainRenderTargetView->Release();
        m_MainRenderTargetView = nullptr;
    }
}

#endif