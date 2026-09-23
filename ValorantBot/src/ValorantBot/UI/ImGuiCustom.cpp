#include "ImGuiCustom.h"

#include <imgui.h>
#include <unordered_map>

namespace ImGui {
    std::unordered_map<const char*, bool> g_ActiveHotkeys;

    // Function to display a hotkey selection element
    bool HotkeySelection(const char* label, Hotkey* hotKey)
    {
        ImGui::Text("%s", label);
        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);

        if (ImGui::Button(ImGui::GetKeyName(hotKey->KeyCode))) {
            g_ActiveHotkeys[label] = !g_ActiveHotkeys[label];
        }

        // Check if the hotkey selection is active
        if (g_ActiveHotkeys[label] && g_ActiveHotkeys.contains(label)) {
            // Iterate over keys and check for pressed keys
            for (ImGuiKey key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key = static_cast<ImGuiKey>(key + 1)) {
                if (ImGui::IsKeyPressed(key) && key != hotKey->KeyCode) {
                    hotKey->KeyCode = key;
                    g_ActiveHotkeys[label] = false;
                }
            }
        }

        return ImGui::IsItemHovered();
    }
}