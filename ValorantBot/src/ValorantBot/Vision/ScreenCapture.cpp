#include "ScreenCapture.h"

#include <d3d11.h>
#include <dxgi.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/base.h>

#include <cstring>

using namespace winrt::Windows::Graphics::Capture;
using namespace winrt::Windows::Graphics::DirectX;
using namespace winrt::Windows::Graphics::DirectX::Direct3D11;

namespace {
    IDirect3DDevice CreateWinRTDevice(ID3D11Device* device)
    {
        winrt::com_ptr<IDXGIDevice> dxgiDevice;

        winrt::check_hresult(device->QueryInterface(IID_PPV_ARGS(dxgiDevice.put())));

        IInspectable* inspectable = nullptr;
        winrt::check_hresult(CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(), &inspectable));

        IDirect3DDevice result {
            inspectable,
            winrt::take_ownership_from_abi
        };

        return result;
    }

    winrt::com_ptr<ID3D11Texture2D> GetTexture(IDirect3DSurface const& surface)
    {
        const winrt::impl::com_ref<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess> access = surface.as<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();

        winrt::com_ptr<ID3D11Texture2D> texture;

        winrt::check_hresult(
            access->GetInterface(
                winrt::guid_of<ID3D11Texture2D>(),
                texture.put_void()));

        return texture;
    }

    HMONITOR MonitorFromPoint(int x, int y)
    {
        const POINT point { x, y };
        return ::MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
    }
}

ScreenCapture::ScreenCapture(int width, int height)
    : m_Width(width), m_Height(height)
{
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    m_BufferSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
    m_Buffer = new uint8_t[m_BufferSize];
    m_Initialized = InitializeD3D();
}

ScreenCapture::~ScreenCapture()
{
    ReleaseCapture();

    m_StagingTexture = nullptr;
    m_Context = nullptr;
    m_Device = nullptr;

    delete[] m_Buffer;
    m_Buffer = nullptr;

    winrt::uninit_apartment();
}

bool ScreenCapture::InitializeD3D()
{
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };

    D3D_FEATURE_LEVEL featureLevel {};
    if (FAILED(D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            flags,
            levels,
            ARRAYSIZE(levels),
            D3D11_SDK_VERSION,
            m_Device.put(),
            &featureLevel,
            m_Context.put()))) {
        return false;
    }

    m_WinRTDevice = CreateWinRTDevice(m_Device.get());

    return true;
}

bool ScreenCapture::CreateStagingTexture(int width, int height)
{
    D3D11_TEXTURE2D_DESC desc {};

    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;

    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

    desc.SampleDesc.Count = 1;

    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    desc.BindFlags = 0;
    desc.MiscFlags = 0;

    m_StagingTexture = nullptr;

    return SUCCEEDED(m_Device->CreateTexture2D(&desc, nullptr, m_StagingTexture.put()));
}

bool ScreenCapture::InitializeCapture(int x, int y)
{
    m_Monitor = MonitorFromPoint(x, y);
    if (!m_Monitor) return false;

    const winrt::com_ptr<IGraphicsCaptureItemInterop> activationFactory = winrt::get_activation_factory<GraphicsCaptureItem, IGraphicsCaptureItemInterop>();

    m_CaptureItem = nullptr;
    if (FAILED(activationFactory->CreateForMonitor(
            m_Monitor,
            winrt::guid_of<GraphicsCaptureItem>(),
            winrt::put_abi(m_CaptureItem)))) {
        return false;
    }

    const winrt::Windows::Graphics::SizeInt32 size = m_CaptureItem.Size();
    if (size.Width <= 0 || size.Height <= 0) return false;

    m_FramePool = Direct3D11CaptureFramePool::CreateFreeThreaded(m_WinRTDevice, DirectXPixelFormat::B8G8R8A8UIntNormalized, 2, size);
    if (!m_FramePool) return false;

    m_CaptureSession = m_FramePool.CreateCaptureSession(m_CaptureItem);
    if (!m_CaptureSession) return false;

    try {
        m_CaptureSession.IsBorderRequired(false);
    } catch (...) {
    }

    m_CaptureSession.StartCapture();

    return true;
}

void ScreenCapture::ReleaseCapture()
{
    m_CaptureSession = nullptr;
    m_FramePool = nullptr;
    m_CaptureItem = nullptr;
    m_Monitor = nullptr;
}

void ScreenCapture::Resize(int width, int height)
{
    if (width <= 0 || height <= 0) return;
    if (width == m_Width && height == m_Height) return;

    m_Width = width;
    m_Height = height;

    const size_t newSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;

    delete[] m_Buffer;

    m_BufferSize = newSize;
    m_Buffer = new uint8_t[m_BufferSize];
}

uint8_t* ScreenCapture::CaptureScreen(int x, int y)
{
    if (!m_Initialized) return nullptr;
    if (!m_CaptureSession && !InitializeCapture(x, y)) return nullptr;

    const Direct3D11CaptureFrame frame = m_FramePool.TryGetNextFrame();
    if (!frame) return m_Buffer;

    const winrt::Windows::Graphics::SizeInt32 frameSize = frame.ContentSize();
    if (frameSize.Width <= 0 || frameSize.Height <= 0) return m_Buffer;

    const winrt::com_ptr<ID3D11Texture2D> source = GetTexture(frame.Surface());
    if (!source) return m_Buffer;

    MONITORINFO monitorInfo {};
    monitorInfo.cbSize = sizeof(monitorInfo);

    if (!GetMonitorInfo(m_Monitor, &monitorInfo)) return m_Buffer;

    const int localX = x - monitorInfo.rcMonitor.left;
    const int localY = y - monitorInfo.rcMonitor.top;

    if (localX < 0 || localY < 0 || localX + m_Width > frameSize.Width || localY + m_Height > frameSize.Height) {
        return m_Buffer;
    }

    // Make sure the staging texture is large enough.
    if (!m_StagingTexture) {
        if (!CreateStagingTexture(m_Width, m_Height)) {
            return m_Buffer;
        }
    }

    D3D11_BOX box {};
    box.left = static_cast<UINT>(localX);
    box.top = static_cast<UINT>(localY);
    box.front = 0;
    box.right = static_cast<UINT>(localX + m_Width);
    box.bottom = static_cast<UINT>(localY + m_Height);
    box.back = 1;

    m_Context->CopySubresourceRegion(
        m_StagingTexture.get(),
        0,
        0,
        0,
        0,
        source.get(),
        0,
        &box);

    D3D11_MAPPED_SUBRESOURCE mapped {};

    if (FAILED(m_Context->Map(
            m_StagingTexture.get(),
            0,
            D3D11_MAP_READ,
            0,
            &mapped))) {
        return m_Buffer;
    }

    const size_t rowBytes = static_cast<size_t>(m_Width) * 4;

    for (int row = 0; row < m_Height; ++row) {
        std::memcpy(m_Buffer + static_cast<size_t>(row) * rowBytes,
            static_cast<const uint8_t*>(mapped.pData) + static_cast<size_t>(row) * mapped.RowPitch,
            rowBytes);
    }

    m_Context->Unmap(m_StagingTexture.get(), 0);

    return m_Buffer;
}