#include "GraphicsContext.h"

#include <Platform/DX11/DX11Context.h>
// #include <Platform/DX11/OpenGLContext.h>

std::unique_ptr<GraphicsContext> GraphicsContext::Create(const Window& window)
{
#ifdef PLATFORM_WINDOWS
    return std::make_unique<DX11Context>(window);
#else
#error "GraphicsContext::Create not implemented for this platform"
    // return std::make_unique<OpenGLContext>(window);
#endif
}