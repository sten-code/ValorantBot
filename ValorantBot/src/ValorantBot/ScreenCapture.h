#pragma once

#include <Windows.h>
#include <cstdint>

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
    int m_Width, m_Height;
    uint8_t* m_Buffer = nullptr;
    size_t m_BufferSize;

    HDC m_hScreenDC;
    HDC m_hCaptureDC;
    HBITMAP m_hBitmap;

    BITMAPINFOHEADER m_BI {};
};
