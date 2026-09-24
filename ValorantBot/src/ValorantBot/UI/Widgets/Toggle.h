#pragma once

#include <imgui.h>

namespace ImGui {
    // Custom switch/toggle widget. Returns true when the value changed.
    bool Toggle(const char* label, bool* enabled);
}