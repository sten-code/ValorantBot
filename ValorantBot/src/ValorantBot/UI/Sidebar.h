#pragma once

#include <imgui.h>

namespace ImGui {
    // Full-width navigation item with icon and active accent bar. Returns true when clicked.
    bool SidebarItem(const char* icon, const char* label, bool active);
}