#include "Window.h"

#ifdef PLATFORM_WINDOWS
#include <Platform/Win32/Win32Window.h>
#endif

std::unique_ptr<Window> Window::Create(const WindowProps& props)
{
#ifdef PLATFORM_WINDOWS
    return std::make_unique<Win32Window>(props);
#else
#error "Platform not supported"
#endif
}
