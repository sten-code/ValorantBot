#pragma once

#include <ValorantBot/Settings.h>

#include <imgui.h>

namespace ImGui {
    // Rounded, elevated panel used for a settings group.
    bool BeginCard(const char* id, const char* title, const char* subtitle = nullptr);
    void EndCard();

    void SectionHeader(const char* label);

    // Label on the left, control on the right. Returns true when the value changed.
    bool ToggleRow(const char* label, bool* enabled);
    bool SliderFloatRow(const char* label, float* value, float min, float max, const char* format = "%.1f");
    bool SliderIntRow(const char* label, int* value, int min, int max, const char* format = "%d");
    bool HotkeyRow(const char* label, Hotkey* hotKey);
    // Input rows take a caller-owned buffer, so each field has its own edit state.
    bool HexInputRow(const char* label, unsigned short* value, char* buf, size_t bufSize);
    bool TextInputRow(const char* label, std::string* value, char* buf, size_t bufSize);
}