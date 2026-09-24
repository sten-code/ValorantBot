#pragma once

#include <ValorantBot/Settings.h>
#include <ValorantBot/Vision/ScreenCapture.h>

#include <chrono>
#include <memory>
#include <string>

#include <Engine/Core/Log.h>
#include <hidapi.h>
#include <opencv2/opencv.hpp>

class AimHandler final {
public:
    explicit AimHandler(Config::Settings& settings, std::shared_ptr<ScreenCapture> capture);
    ~AimHandler();

    void OnFrame();

    void ConnectDevice();

    [[nodiscard]] bool IsDeviceConnected() const { return m_Device != nullptr; }
    [[nodiscard]] std::string GetDeviceName() const { return m_DeviceName; }

private:
    struct DeviceTarget {
        unsigned short vid = 0x413c;
        unsigned short pid = 0x301a;
        std::wstring serial = L"CHIDLC";
    };

    cv::Point FindEnemy(const cv::Mat& image) const;
    void OpenDevice();
    void CloseDevice();
    void UpdateTargetFromSettings();
    bool IsDevicePresent() const;
    void CheckDeviceConnection();

    Config::Settings& m_Settings;
    std::shared_ptr<ScreenCapture> m_Capture;

    hid_device* m_Device = nullptr;
    std::string m_DeviceName;
    DeviceTarget m_ActiveTarget;

    cv::Point m_FOVSize;
    cv::Point m_FOVCenter;
    cv::Point m_CaptureLocation;

    std::chrono::steady_clock::time_point m_LastDeviceCheck;
    std::chrono::nanoseconds m_LastShot;
};