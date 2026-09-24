#pragma once

#include <ValorantBot/Settings.h>

#include <imgui.h>

namespace ImGui {
    const char* HotkeyName(ImGuiKey key);

    // Click to bind a new key, click again or Escape to cancel.
    bool HotkeySelection(const char* label, Hotkey* hotKey);
}