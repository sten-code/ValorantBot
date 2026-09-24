#include "SettingsWindow.h"

#include <ValorantBot/UI/Fonts.h>
#include <ValorantBot/UI/Sidebar.h>
#include <ValorantBot/UI/Theme.h>
#include <ValorantBot/UI/Widgets/Cards.h>

#include <IconsMaterialDesignIcons.h>
#include <imgui.h>

#include <cstddef>

namespace {
    constexpr float SIDEBAR_WIDTH = 180.0f;
    constexpr size_t HEX_BUF_SIZE = 8;
    constexpr size_t SERIAL_BUF_SIZE = 128;
}

void SettingsWindow::Draw(Config::Settings& settings, const bool deviceConnected)
{
    if (!m_Visible) {
        return;
    }

    ImGui::Begin("Settings");

    const ImVec2 windowPos = ImGui::GetWindowPos();
    const ImVec2 regionMin = ImGui::GetWindowContentRegionMin();
    const float sepX = windowPos.x + regionMin.x + SIDEBAR_WIDTH + 4.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 8.0f));
    ImGui::BeginChild("##sidebar", ImVec2(SIDEBAR_WIDTH, 0.0f));
    {
        ImGui::Spacing();
        if (ImGui::SidebarItem(ICON_MDI_CROSSHAIRS, "Aimbot", m_ActiveSection == 0)) m_ActiveSection = 0;
        if (ImGui::SidebarItem(ICON_MDI_TARGET, "Triggerbot", m_ActiveSection == 1)) m_ActiveSection = 1;
        if (ImGui::SidebarItem(ICON_MDI_COGS, "General", m_ActiveSection == 2)) m_ActiveSection = 2;
    }
    ImGui::EndChild();
    ImGui::PopStyleVar();

    ImGui::GetWindowDrawList()->AddLine(
        ImVec2(sepX, windowPos.y + 4.0f),
        ImVec2(sepX, windowPos.y + ImGui::GetWindowHeight() - 4.0f),
        ImGui::ColorConvertFloat4ToU32(Theme::Colors().Border));

    ImGui::SameLine();

    ImGui::BeginChild("##content");
    {
        switch (m_ActiveSection) {
            case 0: DrawAimbotSection(settings); break;
            case 1: DrawTriggerbotSection(settings); break;
            default: DrawGeneralSection(settings, deviceConnected); break;
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void SettingsWindow::DrawAimbotSection(Config::Settings& settings)
{
    if (ImGui::BeginCard("Aimbot", "Aimbot", "Automatically moves your aim toward the nearest enemy head.")) {
        ImGui::ToggleRow("Enabled", &settings.aimbot.enabled);
        ImGui::SliderFloatRow("Smoothing", &settings.aimbot.smoothing, 1.0f, 10.0f, "%.1fx");
        ImGui::SliderIntRow("Vertical offset", &settings.aimbot.yOffset, 0, 100, "%d px");
        ImGui::HotkeyRow("Hold key", &settings.aimbot.key);
    }
    ImGui::EndCard();
}

void SettingsWindow::DrawTriggerbotSection(Config::Settings& settings)
{
    if (ImGui::BeginCard("Triggerbot", "Triggerbot", "Fires when the crosshair is on an enemy.")) {
        ImGui::ToggleRow("Enabled", &settings.triggerbot.enabled);
        ImGui::SliderFloatRow("Range", &settings.triggerbot.range, 0.0f, 20.0f, "%.1f px");
        ImGui::SliderIntRow("Delay", &settings.triggerbot.delayMs, 0, 1000, "%d ms");
    }
    ImGui::EndCard();
}

void SettingsWindow::DrawGeneralSection(Config::Settings& settings, const bool deviceConnected)
{
    if (ImGui::BeginCard("General", "General", "Device and configuration.")) {
        static char s_VidBuf[HEX_BUF_SIZE];
        static char s_PidBuf[HEX_BUF_SIZE];
        static char s_SerialBuf[SERIAL_BUF_SIZE];

        ImGui::SectionHeader("Microcontroller");
        ImGui::HexInputRow("VID", &settings.device.vid, s_VidBuf, HEX_BUF_SIZE);
        ImGui::HexInputRow("PID", &settings.device.pid, s_PidBuf, HEX_BUF_SIZE);
        ImGui::TextInputRow("Serial", &settings.device.serial, s_SerialBuf, SERIAL_BUF_SIZE);

        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, Theme::Colors().Primary);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::Colors().Primary);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.118f, 0.118f, 0.180f, 1.0f));
        if (ImGui::Button("Connect", ImVec2(140.0f, 0.0f))) {
            m_PendingConnectFrames = 1;
        }
        ImGui::PopStyleColor(3);

        if (m_PendingConnectFrames > 0) {
            m_PendingConnectFrames--;
            if (m_PendingConnectFrames == 0 && OnConnect) {
                OnConnect();
            }
        }

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, Theme::Colors().PrimaryDim);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::Colors().Primary);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        const bool saveClicked = ImGui::Button("Save settings", ImVec2(140.0f, 0.0f));
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        if (ImGui::Button("Reset defaults", ImVec2(140.0f, 0.0f))) {
            if (OnReset) OnReset();
        }

        if (saveClicked && OnSave) {
            OnSave();
        }
    }
    ImGui::EndCard();
}