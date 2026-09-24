#include "Theme.h"

#include <algorithm>

namespace {
    ImVec4 Hex(const uint32_t value)
    {
        return ImVec4(
            static_cast<float>((value >> 24) & 0xFF) / 255.0f,
            static_cast<float>((value >> 16) & 0xFF) / 255.0f,
            static_cast<float>((value >> 8) & 0xFF) / 255.0f,
            static_cast<float>(value & 0xFF) / 255.0f);
    }
}

namespace Theme {
    // Catppuccin Mocha palette.
    static Palette s_Palette = {
        .Background = Hex(0x1E1E2EFF), // Base
        .Panel = Hex(0x181825FF), // Mantle
        .Surface = Hex(0x313244FF), // Surface0
        .Border = Hex(0x45475AFF), // Surface1
        .Hover = Hex(0x45475AFF), // Surface1
        .Active = Hex(0x585B70FF), // Surface2
        .Text = Hex(0xCDD6F4FF), // Text
        .TextMuted = Hex(0x7F849CFF), // Overlay1
        .Primary = Hex(0xCBA6F7FF), // Mauve accent
        .PrimaryDim = Hex(0x7A6698FF), // Mauve dimmed
        .Success = Hex(0xA6E3A1FF), // Green
        .Danger = Hex(0xF38BA8FF), // Red
    };

    const Palette& Colors()
    {
        return s_Palette;
    }

    void Apply()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        const Palette& c = s_Palette;

        style.WindowRounding = 8.0f;
        style.ChildRounding = 6.0f;
        style.PopupRounding = 8.0f;
        style.FrameRounding = 6.0f;
        style.ScrollbarRounding = 8.0f;
        style.GrabRounding = 6.0f;
        style.TabRounding = 6.0f;

        style.WindowBorderSize = 1.0f;
        style.ChildBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
        style.FrameBorderSize = 0.0f;
        style.TabBorderSize = 0.0f;

        style.WindowPadding = ImVec2(12.0f, 10.0f);
        style.FramePadding = ImVec2(8.0f, 4.0f);
        style.CellPadding = ImVec2(8.0f, 6.0f);
        style.ItemSpacing = ImVec2(8.0f, 6.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);

        style.TabBarBorderSize = 1.0f;
        style.TabBarOverlineSize = 0.0f;

        // Base colors
        ImVec4* colors = style.Colors;
        colors[ImGuiCol_Text] = c.Text;
        colors[ImGuiCol_TextDisabled] = c.TextMuted;
        colors[ImGuiCol_WindowBg] = c.Background;
        colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_PopupBg] = c.Panel;
        colors[ImGuiCol_Border] = c.Border;
        colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

        // Frames
        colors[ImGuiCol_FrameBg] = c.Panel;
        colors[ImGuiCol_FrameBgHovered] = c.Hover;
        colors[ImGuiCol_FrameBgActive] = c.Active;

        // Title bar (docked windows use tabs)
        colors[ImGuiCol_TitleBg] = c.Background;
        colors[ImGuiCol_TitleBgActive] = c.Panel;
        colors[ImGuiCol_TitleBgCollapsed] = c.Background;
        colors[ImGuiCol_MenuBarBg] = c.Panel;

        // Scrollbar
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_ScrollbarGrab] = c.Border;
        colors[ImGuiCol_ScrollbarGrabHovered] = c.Active;
        colors[ImGuiCol_ScrollbarGrabActive] = c.PrimaryDim;

        // Check / sliders / grabs
        colors[ImGuiCol_CheckMark] = c.Primary;
        colors[ImGuiCol_SliderGrab] = c.Primary;
        colors[ImGuiCol_SliderGrabActive] = c.Primary;

        // Buttons
        colors[ImGuiCol_Button] = c.Surface;
        colors[ImGuiCol_ButtonHovered] = c.Hover;
        colors[ImGuiCol_ButtonActive] = c.Active;

        // Headers (selectable / active nav)
        colors[ImGuiCol_Header] = c.Surface;
        colors[ImGuiCol_HeaderHovered] = c.Hover;
        colors[ImGuiCol_HeaderActive] = c.Active;

        // Separators
        colors[ImGuiCol_Separator] = c.Border;
        colors[ImGuiCol_SeparatorHovered] = c.PrimaryDim;
        colors[ImGuiCol_SeparatorActive] = c.Primary;

        // Resize grip
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_ResizeGripHovered] = c.Primary;
        colors[ImGuiCol_ResizeGripActive] = c.PrimaryDim;

        // Tabs
        colors[ImGuiCol_Tab] = c.Background;
        colors[ImGuiCol_TabHovered] = c.Hover;
        colors[ImGuiCol_TabSelected] = c.Panel;
        colors[ImGuiCol_TabSelectedOverline] = c.Primary;
        colors[ImGuiCol_TabDimmed] = c.Background;
        colors[ImGuiCol_TabDimmedSelected] = c.Panel;
        colors[ImGuiCol_TabDimmedSelectedOverline] = c.Border;

        // Tables
        colors[ImGuiCol_TableHeaderBg] = c.Surface;
        colors[ImGuiCol_TableBorderStrong] = c.Border;
        colors[ImGuiCol_TableBorderLight] = c.Border;
        colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_TableRowBgAlt] = c.Panel;

        // Docking
        colors[ImGuiCol_DockingPreview] = c.Primary;
        colors[ImGuiCol_DockingEmptyBg] = c.Panel;

        // Modal
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);

        // Nav
        colors[ImGuiCol_NavHighlight] = c.Primary;
        colors[ImGuiCol_NavWindowingHighlight] = c.Primary;
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
    }
}