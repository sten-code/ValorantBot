#include "AimHandler.h"

#include <ValorantBot/Bot/InputUtils.h>
#include <ValorantBot/Vision/Detection.h>

#include <Engine/Core/Log.h>

#include <Windows.h>
#include <imgui.h>

#include <cmath>
#include <cwchar>
#include <iostream>
#include <string>

namespace {
    std::string WideToUtf8(const std::wstring& wide)
    {
        if (wide.empty()) {
            return {};
        }
        const int size = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), static_cast<int>(wide.size()), nullptr, 0, nullptr, nullptr);
        std::string result(size, '\0');
        WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), static_cast<int>(wide.size()), result.data(), size, nullptr, nullptr);
        return result;
    }

    std::wstring Utf8ToWide(const std::string& utf8)
    {
        if (utf8.empty()) {
            return {};
        }
        const int size = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.size()), nullptr, 0);
        std::wstring result(size, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.size()), result.data(), size);
        return result;
    }

    bool MatchesSerial(const hid_device_info* info, const std::wstring& serial)
    {
        if (info->serial_number == nullptr) {
            return serial.empty();
        }
        return std::wcscmp(info->serial_number, serial.c_str()) == 0;
    }
}

AimHandler::AimHandler(Config::Settings& settings, std::shared_ptr<ScreenCapture> capture)
    : m_Settings(settings)
    , m_Capture(std::move(capture))
{
    const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    const cv::Point windowSize { screenWidth, screenHeight };
    const cv::Point windowCenter = windowSize / 2.0f;
    m_FOVSize = { m_Capture->GetWidth(), m_Capture->GetHeight() };
    m_FOVCenter = m_FOVSize / 2.0f;
    m_CaptureLocation = windowCenter - m_FOVCenter;

    m_LastShot = std::chrono::system_clock::now().time_since_epoch();
    m_LastDeviceCheck = std::chrono::steady_clock::now();

    UpdateTargetFromSettings();
    OpenDevice();
}

AimHandler::~AimHandler()
{
    CloseDevice();
}

void AimHandler::OnFrame()
{
    CheckDeviceConnection();

    uint8_t* imageData = m_Capture->CaptureScreen(m_CaptureLocation.x, m_CaptureLocation.y);
    const cv::Mat image(m_FOVSize.x, m_FOVSize.y, CV_8UC4, imageData);
    if (image.empty()) {
        std::cerr << "Error: Couldn't load image!" << std::endl;
        return;
    }

    if (GetAsyncKeyState(InputUtils::KeyToVK(m_Settings.aimbot.key.KeyCode)) & 0x8000) {
        // INFO("Aimbot key pressed, searching for enemy...");
        cv::Point target = FindEnemy(image);
        if (target.x != 0 && target.y != 0) {
            target.y += m_Settings.aimbot.yOffset;
            // INFO("Enemy found at: ({}, {})", target.x, target.y);
            // Calculate the location of the enemy relative the crosshair
            target = (target - m_FOVCenter) / m_Settings.aimbot.smoothing;
            if (target.x >= 10) target.x = 10;
            if (target.x <= -10) target.x = -10;
            if (target.y >= 10) target.y = 10;
            if (target.y <= -10) target.y = -10;

            const double distance = std::sqrt(target.x * target.x + target.y * target.y);
            const std::chrono::nanoseconds now = std::chrono::system_clock::now().time_since_epoch();
            if (m_Device && m_Settings.triggerbot.enabled && distance < m_Settings.triggerbot.range && (now - m_LastShot).count() > m_Settings.triggerbot.delayMs) {
                // INFO("Triggerbot activated! Shooting at target: ({}, {})", target.x, target.y);
                m_LastShot = now;
                uint8_t buf[3];
                buf[0] = 0;
                buf[1] = 'C';
                buf[2] = '1';
                if (hid_write(m_Device, buf, sizeof(buf)) < 0) {
                    ERR("hid_write failed: {}", WideToUtf8(hid_error(m_Device)));
                }
            }

            if (m_Device && m_Settings.aimbot.enabled) {
                // INFO("Moving mouse to: ({}, {})", target.x, target.y);
                uint8_t buf[5];
                buf[0] = 0;         // report ID (see below)
                buf[1] = 'M';
                buf[2] = (uint8_t)target.x;
                buf[3] = (uint8_t)target.y;
                buf[4] = 0;         // wheel
                if (hid_write(m_Device, buf, sizeof(buf)) < 0) {
                    ERR("hid_write failed: {}", WideToUtf8(hid_error(m_Device)));
                }
            }
        }
    }
}

cv::Point AimHandler::FindEnemy(const cv::Mat& image) const
{
    // Define the lower and upper bounds for the mask
    static cv::Scalar lowerBound = { 140, 120, 180 };
    static cv::Scalar upperBound = { 160, 200, 255 };

    const std::vector<Contour> contours = Detection::FindContours(image, lowerBound, upperBound);
    const Contour contour = Detection::FindBestContour(contours, m_FOVSize);

    return Detection::FindTop(contour);
}

void AimHandler::OpenDevice()
{
    // Close any previous handle
    CloseDevice();

    hid_device_info* devs = hid_enumerate(m_ActiveTarget.vid, m_ActiveTarget.pid);
    if (!devs) {
        ERR("Failed to enumerate device");
        return;
    }

    std::string chosenPath;
    for (hid_device_info* it = devs; it; it = it->next) {
        // Skip the OS mouse/keyboard collections
        if (it->usage_page == 0x01) {         // Generic Desktop
            continue;                          // 0x01/0x02 = Mouse, 0x01/0x06 = Keyboard
        }
        // Prefer vendor-defined (0xFF00) or any non-Generic-Desktop page
        if (!MatchesSerial(it, m_ActiveTarget.serial)) {
            continue;
        }
        chosenPath = it->path;
        break;
    }
    hid_free_enumeration(devs);

    if (chosenPath.empty()) {
        ERR("No writable HID interface found for this VID/PID/serial");
        m_DeviceName.clear();
        return;
    }

    m_Device = hid_open_path(chosenPath.c_str());
    if (!m_Device) {
        ERR("hid_open_path failed for {}", chosenPath);
        m_DeviceName.clear();
        return;
    }

    const hid_device_info* info = hid_get_device_info(m_Device);
    m_DeviceName = info && info->product_string ? WideToUtf8(info->product_string) : "Unknown";
    INFO("Device connected: {}", m_DeviceName);
}

void AimHandler::UpdateTargetFromSettings()
{
    m_ActiveTarget.vid = m_Settings.device.vid;
    m_ActiveTarget.pid = m_Settings.device.pid;
    m_ActiveTarget.serial = Utf8ToWide(m_Settings.device.serial);
}

void AimHandler::CloseDevice()
{
    if (m_Device) {
        hid_close(m_Device);
        m_Device = nullptr;
    }
    m_DeviceName.clear();
}

bool AimHandler::IsDevicePresent() const
{
    hid_device_info* devices = hid_enumerate(m_ActiveTarget.vid, m_ActiveTarget.pid);
    bool found = false;
    for (const hid_device_info* it = devices; it; it = it->next) {
        if (MatchesSerial(it, m_ActiveTarget.serial)) {
            found = true;
            break;
        }
    }
    hid_free_enumeration(devices);
    return found;
}

void AimHandler::CheckDeviceConnection()
{
    const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    if (now - m_LastDeviceCheck < std::chrono::milliseconds(1000)) {
        return;
    }
    m_LastDeviceCheck = now;

    const bool present = IsDevicePresent();
    if (m_Device && !present) {
        INFO("Device disconnected");
        CloseDevice();
    } else if (!m_Device && present) {
        OpenDevice();
    }
}

void AimHandler::ConnectDevice()
{
    UpdateTargetFromSettings();
    if (m_Device) {
        CloseDevice();
    }
    OpenDevice();
}
