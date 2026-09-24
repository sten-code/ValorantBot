#include "Sidebar.h"

#include <ValorantBot/UI/Theme.h>

#include <imgui_internal.h>

#include <string>

namespace ImGui {
    bool SidebarItem(const char* icon, const char* label, const bool active)
    {
        const float width = ImGui::GetContentRegionAvail().x;
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const ImRect bb(pos, ImVec2(pos.x + width, pos.y + 38.0f));

        ImGui::PushID(label);
        ImGui::InvisibleButton("##nav", bb.GetSize());
        const bool hovered = ImGui::IsItemHovered();
        const bool pressed = ImGui::IsItemClicked();
        ImGui::PopID();

        const Theme::Palette& c = Theme::Colors();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        if (active) {
            drawList->AddRectFilled(bb.Min, bb.Max, ImGui::ColorConvertFloat4ToU32(c.Surface), 6.0f);
            const float accentHeight = 20.0f;
            drawList->AddRectFilled(
                ImVec2(bb.Min.x, bb.Min.y + (bb.GetHeight() - accentHeight) * 0.5f),
                ImVec2(bb.Min.x + 3.0f, bb.Min.y + (bb.GetHeight() + accentHeight) * 0.5f),
                ImGui::ColorConvertFloat4ToU32(c.Primary),
                1.5f);
        } else if (hovered) {
            drawList->AddRectFilled(bb.Min, bb.Max, ImGui::ColorConvertFloat4ToU32(c.Hover), 6.0f);
        }

        const ImU32 textColor = ImGui::ColorConvertFloat4ToU32(active ? c.Primary : (hovered ? c.Text : c.TextMuted));
        const ImVec2 textPos(bb.Min.x + 16.0f, bb.Min.y + (bb.GetHeight() - ImGui::GetTextLineHeight()) * 0.5f);
        drawList->AddText(textPos, textColor, (std::string(icon) + "  " + label).c_str());

        return pressed;
    }
}