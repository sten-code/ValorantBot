#include "MainLayer.h"

#include <Engine/Core/Application.h>
#include <ValorantBot/Detection.h>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <chrono>

static cv::Point FindEnemy(const cv::Mat& image, const cv::Point fovSize)
{
    // Define the lower and upper bounds for the mask
    static cv::Scalar lowerBound = { 140, 120, 180 };
    static cv::Scalar upperBound = { 160, 200, 255 };

    const std::vector<Contour> contours = Detection::FindContours(image, lowerBound, upperBound);
    const Contour contour = Detection::FindBestContour(contours, fovSize);

    return Detection::FindTop(contour);
}

int ImGuiKeyToVK(const ImGuiKey key)
{
    switch (key) {
        case ImGuiKey_Tab: return VK_TAB;
        case ImGuiKey_LeftArrow: return VK_LEFT;
        case ImGuiKey_RightArrow: return VK_RIGHT;
        case ImGuiKey_UpArrow: return VK_UP;
        case ImGuiKey_DownArrow: return VK_DOWN;
        case ImGuiKey_PageUp: return VK_PRIOR;
        case ImGuiKey_PageDown: return VK_NEXT;
        case ImGuiKey_Home: return VK_HOME;
        case ImGuiKey_End: return VK_END;
        case ImGuiKey_Insert: return VK_INSERT;
        case ImGuiKey_Delete: return VK_DELETE;
        case ImGuiKey_Backspace: return VK_BACK;
        case ImGuiKey_Space: return VK_SPACE;
        case ImGuiKey_Enter: return VK_RETURN;
        case ImGuiKey_Escape: return VK_ESCAPE;

        // Left
        case ImGuiKey_LeftCtrl: return VK_LCONTROL;
        case ImGuiKey_LeftShift: return VK_LSHIFT;
        case ImGuiKey_LeftAlt: return VK_LMENU;
        case ImGuiKey_LeftSuper: return VK_LWIN;

        // Right
        case ImGuiKey_RightCtrl: return VK_RCONTROL;
        case ImGuiKey_RightShift: return VK_RSHIFT;
        case ImGuiKey_RightAlt: return VK_RMENU;
        case ImGuiKey_RightSuper: return VK_RWIN;

        case ImGuiKey_Menu: return VK_MENU;

        case ImGuiKey_0: return '0';
        case ImGuiKey_1: return '1';
        case ImGuiKey_2: return '2';
        case ImGuiKey_3: return '3';
        case ImGuiKey_4: return '4';
        case ImGuiKey_5: return '5';
        case ImGuiKey_6: return '6';
        case ImGuiKey_7: return '7';
        case ImGuiKey_8: return '8';
        case ImGuiKey_9: return '9';

        case ImGuiKey_A: return 'A';
        case ImGuiKey_B: return 'B';
        case ImGuiKey_C: return 'C';
        case ImGuiKey_D: return 'D';
        case ImGuiKey_E: return 'E';
        case ImGuiKey_F: return 'F';
        case ImGuiKey_G: return 'G';
        case ImGuiKey_H: return 'H';
        case ImGuiKey_I: return 'I';
        case ImGuiKey_J: return 'J';
        case ImGuiKey_K: return 'K';
        case ImGuiKey_L: return 'L';
        case ImGuiKey_M: return 'M';
        case ImGuiKey_N: return 'N';
        case ImGuiKey_O: return 'O';
        case ImGuiKey_P: return 'P';
        case ImGuiKey_Q: return 'Q';
        case ImGuiKey_R: return 'R';
        case ImGuiKey_S: return 'S';
        case ImGuiKey_T: return 'T';
        case ImGuiKey_U: return 'U';
        case ImGuiKey_V: return 'V';
        case ImGuiKey_W: return 'W';
        case ImGuiKey_X: return 'X';
        case ImGuiKey_Y: return 'Y';
        case ImGuiKey_Z: return 'Z';

        case ImGuiKey_F1: return VK_F1;
        case ImGuiKey_F2: return VK_F2;
        case ImGuiKey_F3: return VK_F3;
        case ImGuiKey_F4: return VK_F4;
        case ImGuiKey_F5: return VK_F5;
        case ImGuiKey_F6: return VK_F6;
        case ImGuiKey_F7: return VK_F7;
        case ImGuiKey_F8: return VK_F8;
        case ImGuiKey_F9: return VK_F9;
        case ImGuiKey_F10: return VK_F10;
        case ImGuiKey_F11: return VK_F11;
        case ImGuiKey_F12: return VK_F12;

        case ImGuiKey_Apostrophe: return VK_OEM_7;
        case ImGuiKey_Comma: return VK_OEM_COMMA;
        case ImGuiKey_Minus: return VK_OEM_MINUS;
        case ImGuiKey_Period: return VK_OEM_PERIOD;
        case ImGuiKey_Slash: return VK_OEM_2;
        case ImGuiKey_Semicolon: return VK_OEM_1;
        case ImGuiKey_Equal: return VK_OEM_PLUS;
        case ImGuiKey_LeftBracket: return VK_OEM_4;
        case ImGuiKey_Backslash: return VK_OEM_5;
        case ImGuiKey_RightBracket: return VK_OEM_6;
        case ImGuiKey_GraveAccent: return VK_OEM_3;
        case ImGuiKey_CapsLock: return VK_CAPITAL;
        case ImGuiKey_ScrollLock: return VK_SCROLL;
        case ImGuiKey_NumLock: return VK_NUMLOCK;
        case ImGuiKey_PrintScreen: return VK_SNAPSHOT;
        case ImGuiKey_Pause: return VK_PAUSE;

        case ImGuiKey_Keypad0: return VK_NUMPAD0;
        case ImGuiKey_Keypad1: return VK_NUMPAD1;
        case ImGuiKey_Keypad2: return VK_NUMPAD2;
        case ImGuiKey_Keypad3: return VK_NUMPAD3;
        case ImGuiKey_Keypad4: return VK_NUMPAD4;
        case ImGuiKey_Keypad5: return VK_NUMPAD5;
        case ImGuiKey_Keypad6: return VK_NUMPAD6;
        case ImGuiKey_Keypad7: return VK_NUMPAD7;
        case ImGuiKey_Keypad8: return VK_NUMPAD8;
        case ImGuiKey_Keypad9: return VK_NUMPAD9;
        case ImGuiKey_KeypadDecimal: return VK_DECIMAL;
        case ImGuiKey_KeypadDivide: return VK_DIVIDE;
        case ImGuiKey_KeypadMultiply: return VK_MULTIPLY;
        case ImGuiKey_KeypadSubtract: return VK_SUBTRACT;
        case ImGuiKey_KeypadAdd: return VK_ADD;
        case ImGuiKey_KeypadEnter: return VK_RETURN;
        case ImGuiKey_KeypadEqual: return VK_OEM_NEC_EQUAL;

        case ImGuiMod_Ctrl: return VK_CONTROL;
        case ImGuiMod_Shift: return VK_SHIFT;
        case ImGuiMod_Alt: return VK_MENU;
        case ImGuiMod_Super: return VK_LWIN;
        case ImGuiKey_MouseLeft: return VK_LBUTTON;
        case ImGuiKey_MouseRight: return VK_RBUTTON;
        case ImGuiKey_MouseMiddle: return VK_MBUTTON;
        case ImGuiKey_MouseX1: return VK_XBUTTON1;
        case ImGuiKey_MouseX2: return VK_XBUTTON2;

        default: return 0;
    }
}

MainLayer::MainLayer(Window& window)
    : Layer("MainLayer")
    , m_Window(window)
    , m_MenuBar(*this)
    , m_TitleBar(window)
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    m_WindowSize = { screenWidth, screenHeight };
    m_WindowCenter = m_WindowSize / 2.0f;
    m_FOVSize = { 400, 400 };
    m_FOVCenter = m_FOVSize / 2.0f;
    m_CaptureLocation = m_WindowCenter - m_FOVCenter;
    m_ScreenCapture = std::make_shared<ScreenCapture>(m_FOVSize.x, m_FOVSize.y);

    m_LastShot = std::chrono::system_clock::now().time_since_epoch();
}

void MainLayer::OnAttach()
{
}

void MainLayer::OnUpdate(float deltaTime)
{
    if (!m_Device) {
        m_Device = hid_open(0x413c, 0x301a, L"CHIDLC");
    }

    uint8_t* imageData = m_ScreenCapture->CaptureScreen(m_CaptureLocation.x, m_CaptureLocation.y);
    const cv::Mat image(m_FOVSize.x, m_FOVSize.y, CV_8UC4, imageData);
    if (image.empty()) {
        std::cerr << "Error: Couldn't load image!" << std::endl;
        return;
    }

    if (GetAsyncKeyState(ImGuiKeyToVK(m_AimbotKey.KeyCode)) & 0x8000) {
        INFO("Aimbot key pressed, searching for enemy...");
        cv::Point centerHead = FindEnemy(image, m_FOVSize);
        if (centerHead.x != 0 && centerHead.y != 0) {
            centerHead.y += m_AimbotYOffset;
            INFO("Enemy found at: ({}, {})", centerHead.x, centerHead.y);
            // Calculate the location of the enemy relative the crosshair
            cv::Point target = (centerHead - m_FOVCenter) / m_AimbotSmoothing;
            if (target.x > 80) target.x = 80;
            if (target.x < -80) target.x = -80;
            if (target.y > 80) target.y = 80;
            if (target.y < -80) target.y = -80;

            const double distance = std::sqrt(target.x * target.x + target.y + target.y);
            const std::chrono::nanoseconds now = std::chrono::system_clock::now().time_since_epoch();
            if (m_Device && m_TriggerbotEnabled && distance < m_TriggerbotRange && (now - m_LastShot).count() > m_TriggerbotDelay) {
                INFO("Triggerbot activated! Shooting at target: ({}, {})", target.x, target.y);
                m_LastShot = now;
                uint8_t buf[3];
                buf[0] = 0;
                buf[1] = 'C';
                buf[2] = '1';
                hid_write(m_Device, buf, sizeof(buf));
            }

            if (m_Device && m_AimbotEnabled) {
                INFO("Moving mouse to: ({}, {})", target.x, target.y);
                uint8_t buf[4];
                buf[0] = 0;
                buf[1] = 'M';
                buf[2] = target.x;
                buf[3] = target.y;
                hid_write(m_Device, buf, sizeof(buf));
            }
        }
    }

    if (m_Device && GetAsyncKeyState(VK_LBUTTON) && m_RecoilControlEnabled) {
        uint8_t buf[4];
        buf[0] = 0;
        buf[1] = 'M';
        buf[2] = 0;
        buf[3] = static_cast<uint8_t>(-m_RecoilControlStrength);
        hid_write(m_Device, buf, sizeof(buf));
    }
}

void MainLayer::OnImGuiRender()
{
    const bool isMaximized = m_Window.IsMaximized();
    ImGui::SetCursorPos(ImVec2 { 40.0f, isMaximized ? 9.0f : 3.0f });

    m_MenuBar.Draw();
    const bool menuBarHovered = ImGui::IsItemHovered();
    const bool titleBarHovered = m_TitleBar.Draw();
    if (menuBarHovered) {
        m_Window.SetIsTitleBarHovered(false);
    } else {
        m_Window.SetIsTitleBarHovered(titleBarHovered);
    }
}

void MainLayer::OnImGuiRenderDock()
{
    ImGui::Begin("Settings");

    ImGui::End();
}

void MainLayer::OnEvent(Event& event)
{
}
