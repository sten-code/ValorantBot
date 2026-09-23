#pragma once

#include <Engine/Core/Layers/Layer.h>
#include <Engine/Core/Window.h>
#include <ValorantBot/ScreenCapture.h>
#include <ValorantBot/UI/ImGuiCustom.h>
#include <ValorantBot/UI/MenuBar.h>
#include <ValorantBot/UI/TitleBar.h>

#include <hidapi.h>
#include <opencv2/opencv.hpp>

class MainLayer final : public Layer {
public:
    explicit MainLayer(Window& window);
    ~MainLayer() override = default;

    void OnAttach() override;
    void OnUpdate(float deltaTime) override;
    void OnImGuiRender() override;
    void OnImGuiRenderDock() override;
    void OnEvent(Event& event) override;

private:
    Window& m_Window;

    MenuBar m_MenuBar;
    TitleBar m_TitleBar;

    hid_device* m_Device = nullptr;
    std::shared_ptr<ScreenCapture> m_ScreenCapture;

    std::chrono::nanoseconds m_LastShot;

    cv::Point m_WindowSize;
    cv::Point m_WindowLocation;
    cv::Point m_WindowCenter;
    cv::Point m_FOVSize;
    cv::Point m_FOVCenter;
    cv::Point m_CaptureLocation;

    bool m_AimbotEnabled = true;
    float m_AimbotSmoothing = 2.0f;
    int m_AimbotYOffset = 15;
    bool m_TriggerbotEnabled = true;
    float m_TriggerbotRange = 5.0f;
    int m_TriggerbotDelay = 200;
    bool m_OutlinesEnabled = true;
    bool m_TracersEnabled = true;
    bool m_FOVVisualEnabled = true;
    bool m_RecoilControlEnabled = false;
    int m_RecoilControlStrength = -20;
    Hotkey m_AimbotKey = { ImGuiMod_Alt };
};
