#include "Toggle.h"

#include <ValorantBot/UI/Theme.h>

#include <imgui_internal.h>

namespace {
    constexpr float TRACK_WIDTH = 42.0f;
    constexpr float TRACK_HEIGHT = 22.0f;
    constexpr float KNOB_SIZE = 16.0f;
    constexpr float KNOB_PADDING = 3.0f;
    constexpr float ANIM_SPEED = 10.0f;
}

namespace ImGui {
    bool Toggle(const char* label, bool* enabled)
    {
        const ImGuiID id = ImGui::GetID(label);
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const ImRect bb(pos, ImVec2(pos.x + TRACK_WIDTH, pos.y + TRACK_HEIGHT));

        ImGui::PushID(label);
        ImGui::InvisibleButton("##toggle", ImVec2(TRACK_WIDTH, TRACK_HEIGHT));
        bool changed = false;
        if (ImGui::IsItemClicked()) {
            *enabled = !*enabled;
            changed = true;
        }

        const bool hovered = ImGui::IsItemHovered();
        ImGuiStorage* storage = ImGui::GetStateStorage();
        float t = storage->GetFloat(id, *enabled ? 1.0f : 0.0f);
        const float target = *enabled ? 1.0f : 0.0f;
        t += (target - t) * ImMin(1.0f, ImGui::GetIO().DeltaTime * ANIM_SPEED);
        storage->SetFloat(id, t);

        const Theme::Palette& c = Theme::Colors();
        ImU32 trackColor = ImGui::ColorConvertFloat4ToU32(*enabled
                ? (hovered ? c.Primary : c.PrimaryDim)
                : (hovered ? c.Hover : c.Border));
        ImU32 knobColor = ImGui::ColorConvertFloat4ToU32(c.Surface);

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(bb.Min, bb.Max, trackColor, TRACK_HEIGHT * 0.5f);
        const float knobHalf = KNOB_SIZE * 0.5f;
        const float knobX = bb.Min.x + KNOB_PADDING + knobHalf + t * (TRACK_WIDTH - KNOB_SIZE - KNOB_PADDING * 2.0f);
        const float knobY = bb.Min.y + (TRACK_HEIGHT - KNOB_SIZE) * 0.5f + knobHalf;
        drawList->AddCircleFilled(ImVec2(knobX, knobY), knobHalf, knobColor);

        ImGui::PopID();
        return changed;
    }
}