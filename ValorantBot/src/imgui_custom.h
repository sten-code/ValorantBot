#pragma once

#include <imgui.h>

struct Hotkey
{
    ImGuiKey KeyCode;
};

namespace ImGui
{
    bool HotkeySelection(const char* label, Hotkey* hotKey);
}