#include "TitleBar.h"

#include <ValorantBot/UI/Fonts.h>
#include <Engine/Core/Application.h>

#include <IconsCodicons.h>
#include <imgui.h>

bool TitleBar::Draw() const
{
    const bool isMaximized = m_Window.IsMaximized();
    // const bool isMaximized = false;
    const float titlebarVerticalOffset = isMaximized ? 6.0f : 0.0f;
    const float width = ImGui::GetContentRegionAvail().x;
    const float buttonRightOffset = isMaximized ? 0.0f : 0.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 0.0f, 0.0f });
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

    // Title bar drag area
    ImGui::SetCursorPos(ImVec2(0.0f, titlebarVerticalOffset));
    float x = ImGui::GetCursorScreenPos().x;
    float y = ImGui::GetCursorScreenPos().y;
    ImGui::GetWindowDrawList()->AddLine(
        ImVec2(x, y + TITLEBAR_HEIGHT),
        ImVec2(x + width, y + TITLEBAR_HEIGHT),
        0xFF141010);

    ImGui::InvisibleButton("##titleBarDragZone",
        ImVec2(width - BUTTON_WIDTH * 3 + buttonRightOffset, TITLEBAR_HEIGHT));
    bool titleBarHovered = ImGui::IsItemHovered();

    Fonts::Push(Fonts::Type::Codicon);
    ImGui::PushStyleColor(ImGuiCol_Button, 0);

    // Minimize Button
    ImGui::SetCursorPos(ImVec2(width - BUTTON_WIDTH * 3 + buttonRightOffset, titlebarVerticalOffset));
    if (ImGui::Button(ICON_CI_CHROME_MINIMIZE, ImVec2(BUTTON_WIDTH, TITLEBAR_HEIGHT))) {
        m_Window.Minimize();
    }

    // Maximize Button
    ImGui::SetCursorPos(ImVec2(width - BUTTON_WIDTH * 2 + buttonRightOffset, titlebarVerticalOffset));
    const char* maximizeIcon = m_Window.IsMaximized() ? ICON_CI_CHROME_RESTORE : ICON_CI_CHROME_MAXIMIZE;
    if (ImGui::Button(maximizeIcon, ImVec2(BUTTON_WIDTH, TITLEBAR_HEIGHT))) {
        isMaximized ? m_Window.Restore() : m_Window.Maximize();
    }

    // Close Button
    ImGui::SetCursorPos(ImVec2(width - BUTTON_WIDTH + buttonRightOffset, titlebarVerticalOffset));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0xc42b1cff);
    if (ImGui::Button(ICON_CI_CHROME_CLOSE, ImVec2(BUTTON_WIDTH, TITLEBAR_HEIGHT))) {
        m_Window.Close();
    }
    ImGui::PopStyleColor();

    ImGui::PopStyleColor();
    Fonts::Pop();
    ImGui::PopStyleVar(2);

    return titleBarHovered;
}