#pragma once

#include <ValorantBot/Settings.h>

#include <functional>
#include <imgui.h>

class SettingsWindow {
public:
    SettingsWindow() = default;

    void Draw(Config::Settings& settings, bool deviceConnected);

    void SetVisible(bool visible) { m_Visible = visible; }
    [[nodiscard]] bool IsVisible() const { return m_Visible; }
    void ToggleVisible() { m_Visible = !m_Visible; }

    std::function<void()> OnSave;
    std::function<void()> OnReset;
    std::function<void()> OnConnect;

private:
    void DrawAimbotSection(Config::Settings& settings);
    void DrawTriggerbotSection(Config::Settings& settings);
    void DrawGeneralSection(Config::Settings& settings, bool deviceConnected);

    int m_ActiveSection = 0;
    int m_PendingConnectFrames = 0;
    bool m_Visible = true;
};