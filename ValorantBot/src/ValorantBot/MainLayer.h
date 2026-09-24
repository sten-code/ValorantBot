#pragma once

#include <Engine/Core/Layers/Layer.h>
#include <Engine/Core/Window.h>
#include <ValorantBot/Bot/AimHandler.h>
#include <ValorantBot/Settings.h>
#include <ValorantBot/UI/MenuBar.h>
#include <ValorantBot/UI/SettingsWindow.h>
#include <ValorantBot/UI/TitleBar.h>
#include <ValorantBot/Vision/ScreenCapture.h>

#include <memory>

class MainLayer final : public Layer {
public:
    static constexpr float STATUS_BAR_HEIGHT = 28.0f;

    explicit MainLayer(Window& window);
    ~MainLayer() override = default;

    void OnAttach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnImGuiRenderDock() override;
    void OnEvent(Event& event) override;

    void SaveConfig() const;
    void ResetConfig();
    void ToggleSettingsWindow() { m_SettingsWindow.ToggleVisible(); }
    void ConnectDevice() { m_AimHandler.ConnectDevice(); }
    [[nodiscard]] bool IsSettingsVisible() const { return m_SettingsWindow.IsVisible(); }
    [[nodiscard]] bool IsDeviceConnected() const { return m_AimHandler.IsDeviceConnected(); }

private:
    void DrawStatusBar() const;

private:
    Window& m_Window;

    MenuBar m_MenuBar;
    TitleBar m_TitleBar;
    SettingsWindow m_SettingsWindow;

    Config::Settings m_Settings;
    std::shared_ptr<ScreenCapture> m_ScreenCapture;
    AimHandler m_AimHandler;
};