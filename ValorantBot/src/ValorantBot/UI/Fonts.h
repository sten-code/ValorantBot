#pragma once

#include <imgui.h>

class Fonts {
public:
    enum class Type {
        FontAwesome6,
        Codicon,
        MaterialDesignIcons,
        JetBrainsMono,
        Header,
    };

    static void LoadFonts();
    static void Push(Type font, float scale = 1.0f);
    static void Pop();
    static ImFont* GetFont(Type font);

private:
    static ImFont* s_FontAwesome6;
    static ImFont* s_Codicon;
    static ImFont* s_MaterialDesignIcons;
    static ImFont* s_JetBrainsMono;
    static ImFont* s_Header;
};