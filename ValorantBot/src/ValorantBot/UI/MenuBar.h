#pragma once

#include <imgui_internal.h>

class MainLayer;

class MenuBar {
public:
    explicit MenuBar(MainLayer& mainLayer);

    void Draw();

private:
    bool BeginMenubar(const ImRect& barRectangle);
    void EndMenubar();

private:
    MainLayer& m_MainLayer;
};