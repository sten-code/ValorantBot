#pragma once

#include <Engine/Core/Window.h>

class TitleBar {
public:
    static constexpr float TITLEBAR_HEIGHT = 35.0f;
    static constexpr float BUTTON_WIDTH = 45.0f;

    explicit TitleBar(Window& window)
        : m_Window(window)  {}

    bool Draw() const; // Returns true if hovered

private:
    Window& m_Window;
};