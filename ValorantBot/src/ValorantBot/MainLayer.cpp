#include "MainLayer.h"

#include <ValorantBot/SettingsSerializer.h>
#include <ValorantBot/UI/Theme.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace {
    Config::Settings& LoadSettings(Config::Settings& settings)
    {
        SettingsSerializer::Load(settings);
        return settings;
    }
}

MainLayer::MainLayer(Window& window)
    : Layer("MainLayer")
    , m_Window(window)
    , m_MenuBar(*this)
    , m_TitleBar(window)
    , m_ScreenCapture(std::make_shared<ScreenCapture>(400, 400))
    , m_AimHandler(LoadSettings(m_Settings), m_ScreenCapture)
{
    m_SettingsWindow.OnSave = [this]() {
        SettingsSerializer::Save(m_Settings);
    };
    m_SettingsWindow.OnReset = [this]() {
        m_Settings.Reset();
        SettingsSerializer::Save(m_Settings);
    };
    m_SettingsWindow.OnConnect = [this]() {
        ConnectDevice();
    };
}

void MainLayer::OnAttach()
{
}

void MainLayer::OnUpdate(float deltaTime)
{
    m_AimHandler.OnFrame();
}

void MainLayer::OnImGuiRender()
{
    const bool isMaximized = m_Window.IsMaximized();
    ImGui::SetCursorPos(ImVec2 { 40.0f, isMaximized ? 9.0f : 3.0f });

    // First time the "Settings" window has ever been drawn, dock it into the main dockspace.
    static bool s_DockInitialized = false;
    if (!s_DockInitialized) {
        ImGuiWindow* settingsWindow = ImGui::FindWindowByName("Settings");
        if (settingsWindow && settingsWindow->DockId == 0) {
            ImGuiID dockspaceId = ImGui::GetID("MainDockSpace");
            ImGui::DockBuilderRemoveNode(dockspaceId);
            ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->Size);

            ImGuiID rightId;
            ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Right, 0.24f, &rightId, &dockspaceId);
            ImGui::DockBuilderDockWindow("Settings", rightId);
            ImGui::DockBuilderFinish(dockspaceId);

            s_DockInitialized = true;
        }
    }

    m_MenuBar.Draw();
    const bool menuBarHovered = ImGui::IsItemHovered();
    const bool titleBarHovered = m_TitleBar.Draw();
    if (menuBarHovered) {
        m_Window.SetIsTitleBarHovered(false);
    } else {
        m_Window.SetIsTitleBarHovered(titleBarHovered);
    }

    DrawStatusBar();
}

void MainLayer::DrawStatusBar() const
{
    const ImVec2 windowPos = ImGui::GetWindowPos();
    const ImVec2 windowSize = ImGui::GetWindowSize();
    const float y0 = windowPos.y + windowSize.y - STATUS_BAR_HEIGHT;

    const Theme::Palette& colors = Theme::Colors();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    drawList->AddRectFilled(
        ImVec2(windowPos.x, y0),
        ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y),
        ImGui::ColorConvertFloat4ToU32(colors.Panel));
    drawList->AddLine(
        ImVec2(windowPos.x, y0),
        ImVec2(windowPos.x + windowSize.x, y0),
        ImGui::ColorConvertFloat4ToU32(colors.Border));

    const bool connected = m_AimHandler.IsDeviceConnected();
    const ImU32 dotColor = ImGui::ColorConvertFloat4ToU32(connected ? colors.Success : colors.Danger);
    const float pad = 12.0f;
    const float textY = y0 + (STATUS_BAR_HEIGHT - ImGui::GetTextLineHeight()) * 0.5f;
    drawList->AddCircleFilled(
        ImVec2(windowPos.x + pad + 6.0f, textY + ImGui::GetTextLineHeight() * 0.5f),
        5.0f,
        dotColor);

    char label[128];
    ImFormatString(label, IM_ARRAYSIZE(label), "Device: %s", connected ? m_AimHandler.GetDeviceName() : "not connected");
    drawList->AddText(
        ImVec2(windowPos.x + pad + 18.0f, textY),
        ImGui::ColorConvertFloat4ToU32(colors.TextMuted),
        label);
}

void MainLayer::OnImGuiRenderDock()
{
    m_SettingsWindow.Draw(m_Settings, m_AimHandler.IsDeviceConnected());
}

void MainLayer::OnEvent(Event& event)
{
}

void MainLayer::SaveConfig() const
{
    SettingsSerializer::Save(m_Settings);
}

void MainLayer::ResetConfig()
{
    m_Settings.Reset();
    SettingsSerializer::Save(m_Settings);
}