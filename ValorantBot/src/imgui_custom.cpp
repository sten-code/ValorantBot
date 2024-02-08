#include "imgui_custom.h"

#include <iostream>
#include <unordered_map>
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace ImGui
{
    std::unordered_map<const char*, bool> g_ActiveHotkeys;

    // Function to display a hotkey selection element
    bool HotkeySelection(const char* label, Hotkey* hotKey)
    {
        ImGui::Text("%s", label);
        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);

        if (ImGui::Button(ImGui::GetKeyName(hotKey->KeyCode)))
        {
            g_ActiveHotkeys[label] = !g_ActiveHotkeys[label];
        }

        // Check if the hotkey selection is active
        if (g_ActiveHotkeys[label] && g_ActiveHotkeys.find(label) != g_ActiveHotkeys.end())
        {
            // Iterate over keys and check for pressed keys
            for (int i = 0; i < IM_ARRAYSIZE(ImGui::GetIO().KeysDown); i++)
            {
                if (ImGui::IsKeyDown(i) && i != hotKey->KeyCode)
                {
                    hotKey->KeyCode = i;
                    g_ActiveHotkeys[label] = false;
                }
            }
        }

        return ImGui::IsItemHovered();
    }
}