#include "Fonts.h"

#include <IconsCodicons.h>
#include <IconsFontAwesome6.h>
#include <IconsMaterialDesignIcons.h>

#include <Engine/Core/Log.h>

ImFont* Fonts::s_FontAwesome6 = nullptr;
ImFont* Fonts::s_Codicon = nullptr;
ImFont* Fonts::s_MaterialDesignIcons = nullptr;
ImFont* Fonts::s_JetBrainsMono = nullptr;

void Fonts::LoadFonts()
{
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig cfg;
    cfg.PixelSnapH = true;

    // Font Awesome 6
    cfg.MergeMode = false;
    s_FontAwesome6 = io.Fonts->AddFontFromFileTTF(
        "Resources/Fonts/NotoSans-Regular.ttf",
        15.0f, &cfg);
    cfg.MergeMode = true;
    io.Fonts->AddFontFromFileTTF(
        "Resources/Fonts/Font Awesome 6 Pro-Light-300.otf",
        13.0f, &cfg);

    // Codicon
    cfg.MergeMode = false;
    s_Codicon = io.Fonts->AddFontFromFileTTF(
        "Resources/Fonts/NotoSans-Regular.ttf",
        15.0f, &cfg);
    cfg.MergeMode = true;
    cfg.GlyphOffset = ImVec2(0.0f, 3.0f);
    io.Fonts->AddFontFromFileTTF(
        "Resources/Fonts/" FONT_ICON_FILE_NAME_CI,
        15.0f, &cfg);
    cfg.GlyphOffset = ImVec2(0.0f, 0.0f);

    // Material Design Icons
    cfg.MergeMode = false;
    s_MaterialDesignIcons = io.Fonts->AddFontFromFileTTF(
        "Resources/Fonts/NotoSans-Regular.ttf",
        15.0f, &cfg);
    cfg.MergeMode = true;
    cfg.GlyphOffset = ImVec2(0.0f, 1.0f);
    io.Fonts->AddFontFromFileTTF(
        "Resources/Fonts/" FONT_ICON_FILE_NAME_MDI,
        15.0f, &cfg);
    cfg.GlyphOffset = ImVec2(0.0f, 0.0f);

    // JetBrains Mono
    cfg.MergeMode = false;
    s_JetBrainsMono = io.Fonts->AddFontFromFileTTF(
        "Resources/Fonts/JetBrainsMono.ttf",
        15.0f, &cfg);

    io.FontDefault = s_MaterialDesignIcons;
}

void Fonts::Push(Type font, float scale)
{
    ImFont* imFont = GetFont(font);
    if (!imFont) {
        ERR("Fonts::Push: Font does not exist!");
        return;
    }
    ImGui::PushFont(imFont, ImGui::GetFontSize() * scale);
}

void Fonts::Pop()
{
    ImGui::PopFont();
}

ImFont* Fonts::GetFont(Type font)
{
    switch (font) {
        case Type::FontAwesome6: return s_FontAwesome6;
        case Type::Codicon: return s_Codicon;
        case Type::MaterialDesignIcons: return s_MaterialDesignIcons;
        case Type::JetBrainsMono: return s_JetBrainsMono;
    }
    return nullptr;
}