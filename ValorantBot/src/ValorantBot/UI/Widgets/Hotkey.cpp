#include "Hotkey.h"

#include <ValorantBot/UI/Theme.h>

#include <unordered_map>

namespace ImGui {
    static std::unordered_map<ImGuiID, bool> s_ActiveHotkeys;

    const char* HotkeyName(const ImGuiKey key)
    {
        switch (key) {
            case ImGuiMod_Ctrl: return "Ctrl";
            case ImGuiMod_Shift: return "Shift";
            case ImGuiMod_Alt: return "Alt";
            case ImGuiMod_Super: return "Win";
            default: break;
        }

        if (key >= ImGuiKey_NamedKey_BEGIN && key < ImGuiKey_NamedKey_END) {
            return ImGui::GetKeyName(key);
        }
        return "None";
    }

    bool HotkeySelection(const char* label, Hotkey* hotKey)
    {
        const ImGuiID id = ImGui::GetID(label);
        bool& isBinding = s_ActiveHotkeys[id];

        const char* buttonLabel = isBinding ? "Binding..." : HotkeyName(hotKey->KeyCode);

        ImGui::PushID(label);
        ImGui::SetNextItemWidth(130.0f);
        if (isBinding) {
            ImGui::PushStyleColor(ImGuiCol_Button, Theme::Colors().PrimaryDim);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, Theme::Colors().Panel);
            ImGui::PushStyleColor(ImGuiCol_Text, Theme::Colors().Text);
        }

        const bool clicked = ImGui::Button(buttonLabel);
        ImGui::PopStyleColor(2);
        ImGui::PopID();

        if (clicked) {
            isBinding = !isBinding;
        }

        if (isBinding) {
            if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                isBinding = false;
            } else {
                for (ImGuiKey key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key = static_cast<ImGuiKey>(key + 1)) {
                    if (ImGui::IsKeyPressed(key) && key != ImGuiKey_MouseLeft) {
                        hotKey->KeyCode = key;
                        isBinding = false;
                        break;
                    }
                }
            }
        }

        return clicked || isBinding;
    }
}