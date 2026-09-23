#include "ScreenCapture.h"

#include <Windows.h>
#include <cstdint>

ScreenCapture::ScreenCapture(int width, int height)
    : m_Width(width), m_Height(height)
{
    m_hScreenDC = GetDC(nullptr);
    m_hCaptureDC = CreateCompatibleDC(m_hScreenDC);
    m_hBitmap = CreateCompatibleBitmap(m_hScreenDC, width, height);

    SelectObject(m_hCaptureDC, m_hBitmap);
    m_BufferSize = width * height * 4;
    m_Buffer = new uint8_t[m_BufferSize];

    m_BI.biSize = sizeof(BITMAPINFOHEADER);
    m_BI.biWidth = width;
    m_BI.biHeight = -height; // Invert the image vertically
    m_BI.biPlanes = 1;
    m_BI.biBitCount = 32;
    m_BI.biCompression = BI_RGB;
    m_BI.biSizeImage = 0;
    m_BI.biXPelsPerMeter = 0;
    m_BI.biYPelsPerMeter = 0;
    m_BI.biClrUsed = 0;
    m_BI.biClrImportant = 0;
}

ScreenCapture::~ScreenCapture()
{
    // Cleanup
    delete[] m_Buffer;
    DeleteObject(m_hBitmap);
    DeleteDC(m_hCaptureDC);
    ReleaseDC(nullptr, m_hScreenDC);
}

void ScreenCapture::Resize(int width, int height)
{
    m_Width = width;
    m_Height = height;
    m_hBitmap = CreateCompatibleBitmap(m_hScreenDC, width, height);
    m_BufferSize = width * height * 4;

    delete[] m_Buffer;
    m_Buffer = new uint8_t[m_BufferSize];
}

uint8_t* ScreenCapture::CaptureScreen(int x, int y)
{
    BitBlt(m_hCaptureDC, 0, 0, m_Width, m_Height, m_hScreenDC, x, y, SRCCOPY);
    GetDIBits(m_hCaptureDC, m_hBitmap, 0, m_Height, m_Buffer, reinterpret_cast<BITMAPINFO*>(&m_BI), DIB_RGB_COLORS);
    return m_Buffer;
}
