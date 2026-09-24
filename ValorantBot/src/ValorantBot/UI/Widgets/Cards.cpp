#include "Cards.h"

#include "Hotkey.h"
#include "Toggle.h"
#include <ValorantBot/UI/Fonts.h>
#include <ValorantBot/UI/Theme.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace ImGui {
    bool BeginCard(const char* id, const char* title, const char* subtitle)
    {
        const Theme::Palette& c = Theme::Colors();

        ImGui::PushID(id);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, c.Surface);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 12.0f));

        const float width = ImGui::GetContentRegionAvail().x;
        const bool open = ImGui::BeginChild("##card", ImVec2(width, 0.0f), ImGuiChildFlags_AlwaysUseWindowPadding);

        if (open) {
            Fonts::Push(Fonts::Type::Header, 1.05f);
            ImGui::PushStyleColor(ImGuiCol_Text, c.Text);
            ImGui::TextUnformatted(title);
            ImGui::PopStyleColor();
            Fonts::Pop();

            if (subtitle && subtitle[0] != '\0') {
                ImGui::PushStyleColor(ImGuiCol_Text, c.TextMuted);
                Fonts::Push(Fonts::Type::Header, 0.8f);
                ImGui::TextUnformatted(subtitle);
                Fonts::Pop();
                ImGui::PopStyleColor();
            }

            ImGui::PushStyleColor(ImGuiCol_Separator, c.Border);
            ImGui::Separator();
            ImGui::PopStyleColor();
            ImGui::Spacing();
        }

        return open;
    }

    void EndCard()
    {
        ImGui::EndChild();
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor();
        ImGui::PopID();
    }

    void SectionHeader(const char* label)
    {
        const Theme::Palette& c = Theme::Colors();
        Fonts::Push(Fonts::Type::Header, 1.2f);
        ImGui::PushStyleColor(ImGuiCol_Text, c.Text);
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();
        Fonts::Pop();

        ImGui::PushStyleColor(ImGuiCol_Separator, c.Border);
        ImGui::Separator();
        ImGui::PopStyleColor();
    }

    bool ToggleRow(const char* label, bool* enabled)
    {
        ImGui::PushID(label);
        ImGui::AlignTextToFramePadding();
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::Colors().Text);
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();
        ImGui::SameLine();

        const float toggleX = ImGui::GetContentRegionAvail().x - 42.0f;
        if (toggleX > ImGui::GetCursorPosX()) {
            ImGui::Dummy(ImVec2(toggleX - ImGui::GetCursorPosX(), 0.0f));
            ImGui::SameLine();
        }

        const bool changed = ImGui::Toggle("##switch", enabled);
        ImGui::PopID();
        return changed;
    }

    bool SliderFloatRow(const char* label, float* value, const float min, const float max, const char* format)
    {
        ImGui::PushID(label);
        ImGui::AlignTextToFramePadding();
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::Colors().Text);
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        ImGui::SameLine();

        const float sliderWidth = 220.0f;
        const float offset = ImGui::GetContentRegionAvail().x - sliderWidth;
        if (offset > 0.0f) {
            ImGui::Dummy(ImVec2(offset, 0.0f));
            ImGui::SameLine();
        }

        ImGui::SetNextItemWidth(sliderWidth);
        const bool changed = ImGui::SliderFloat("##slider", value, min, max, format);
        ImGui::PopID();
        return changed;
    }

    bool SliderIntRow(const char* label, int* value, const int min, const int max, const char* format)
    {
        ImGui::PushID(label);
        ImGui::AlignTextToFramePadding();
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::Colors().Text);
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();

        ImGui::SameLine();

        const float sliderWidth = 220.0f;
        const float offset = ImGui::GetContentRegionAvail().x - sliderWidth;
        if (offset > 0.0f) {
            ImGui::Dummy(ImVec2(offset, 0.0f));
            ImGui::SameLine();
        }

        ImGui::SetNextItemWidth(sliderWidth);
        const bool changed = ImGui::SliderInt("##slider", value, min, max, format);
        ImGui::PopID();
        return changed;
    }

    bool HotkeyRow(const char* label, Hotkey* hotKey)
    {
        ImGui::PushID(label);
        ImGui::AlignTextToFramePadding();
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::Colors().Text);
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();
        ImGui::SameLine();

        const float buttonWidth = 130.0f;
        const float offset = ImGui::GetContentRegionAvail().x - buttonWidth;
        if (offset > 0.0f) {
            ImGui::Dummy(ImVec2(offset, 0.0f));
            ImGui::SameLine();
        }

        const bool changed = ImGui::HotkeySelection("##key", hotKey);
        ImGui::PopID();
        return changed;
    }

    bool HexInputRow(const char* label, unsigned short* value, char* buf, const size_t bufSize)
    {
        const float inputWidth = 110.0f;

        ImGui::PushID(label);
        ImGui::AlignTextToFramePadding();
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::Colors().Text);
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();
        ImGui::SameLine();

        const float offset = ImGui::GetContentRegionAvail().x - inputWidth;
        if (offset > 0.0f) {
            ImGui::Dummy(ImVec2(offset, 0.0f));
            ImGui::SameLine();
        }

        ImGui::SetNextItemWidth(inputWidth);
        const bool edited = ImGui::InputText("##hex", buf, bufSize, ImGuiInputTextFlags_CharsHexadecimal);
        bool changed = edited;
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            *value = static_cast<unsigned short>(std::strtoul(buf, nullptr, 16));
            changed = true;
        }
        if (!ImGui::IsItemActive()) {
            std::snprintf(buf, bufSize, "%04X", *value);
        }
        ImGui::PopID();
        return changed;
    }

    bool TextInputRow(const char* label, std::string* value, char* buf, const size_t bufSize)
    {
        const float inputWidth = 220.0f;

        ImGui::PushID(label);
        ImGui::AlignTextToFramePadding();
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::Colors().Text);
        ImGui::TextUnformatted(label);
        ImGui::PopStyleColor();
        ImGui::SameLine();

        const float offset = ImGui::GetContentRegionAvail().x - inputWidth;
        if (offset > 0.0f) {
            ImGui::Dummy(ImVec2(offset, 0.0f));
            ImGui::SameLine();
        }

        ImGui::SetNextItemWidth(inputWidth);
        const bool edited = ImGui::InputText("##text", buf, bufSize);
        bool changed = edited;
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            *value = buf;
            changed = true;
        }
        if (!ImGui::IsItemActive()) {
            std::strncpy(buf, value->c_str(), bufSize - 1);
            buf[bufSize - 1] = '\0';
        }
        ImGui::PopID();
        return changed;
    }
}