#include "screen.h"
#include "window.h"
#include "gui.h"
#include "globals.h"
#include "detection.h"

#include <hidapi.h>
#include <opencv2/opencv.hpp>
#pragma comment(lib, "OneCore.lib")
#include <Windows.h>
#include <dbt.h>
#include <tchar.h>


#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>

cv::Point FindEnemy(cv::Mat& image);
int ImGuiKeyToVK(ImGuiKey key);

static bool s_AimbotEnabled = true;
static float s_AimbotSmoothing = 2.0f;
static int s_AimbotYOffset = 15;
static bool s_TriggerbotEnabled = true;
static float s_TriggerbotRange = 5.0f;
static int s_TriggerbotDelay = 200;
static bool s_OutlinesEnabled = true;
static bool s_TracersEnabled = true;
static bool s_FOVVisualEnabled = true;
static bool s_RecoilControlEnabled = false;
static int s_RecoilControlStrength = -20;
static Hotkey s_AimbotKey = { ImGui::GetKeyIndex(ImGuiKey_ModAlt) };

#define MAX_STR 255

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) 
{
    if (event == EVENT_SYSTEM_FOREGROUND) 
    {
        // Focused window changed
        char windowTitle[256];
        GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));
        std::string title = windowTitle;

        Window::SetEnabled((title.find("VALORANT") != std::string::npos) || hwnd == Window::GetWindow());
    }
}

bool IsValorantForeground() 
{
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL)
        return false; // No foreground window
 
    char windowTitle[256];
    GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));

    std::string title = windowTitle;
    return (title.find("VALORANT") != std::string::npos);
}

int main(int argc, char* argv[])
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    Globals::s_WindowRect = { 0, 0, screenWidth, screenHeight };
    
    // Define the region to capture
    Globals::s_WindowSize = { Globals::s_WindowRect.right - Globals::s_WindowRect.left, Globals::s_WindowRect.bottom - Globals::s_WindowRect.top };
    Globals::s_WindowSize = { 2560, 1600 };
    Globals::s_WindowLocation = { Globals::s_WindowRect.left, Globals::s_WindowRect.top };
    Globals::s_WindowCenter = Globals::s_WindowLocation + Globals::s_WindowSize / 2.0f;
    Globals::s_FOVSize = { 400, 400 };
    Globals::s_FOVCenter = Globals::s_FOVSize / 2.0f;
    Globals::s_CaptureLocation = Globals::s_WindowCenter - Globals::s_FOVCenter;

    // Initialize a screen capture
    ScreenCapture screen(Globals::s_FOVSize.x, Globals::s_FOVSize.y);
    hid_device* handle = nullptr;

    Window::Setup("Valorant Bot", "Valorant Bot Class");
    //Window::SetEnabled(IsValorantForeground());
    //Window::SetOpen(false);
    Window::SetEnabled(true);
    Window::SetOpen(true);
    Gui gui;
    Menu& hacksMenu = gui.AddTab("B", "Hacks");

    SubMenu& aimbotMenu = hacksMenu.AddSubMenu("Aimbot");
    aimbotMenu.AddSetting("Enabled", &s_AimbotEnabled);
    aimbotMenu.AddSetting("Smoothing", &s_AimbotSmoothing, 0.0f, 10.0f);
    aimbotMenu.AddSetting("Y Offset", &s_AimbotYOffset, 0, 100);
    aimbotMenu.AddSetting("Aimbot Key", &s_AimbotKey);

    SubMenu& triggerbotMenu = hacksMenu.AddSubMenu("Triggerbot");
    triggerbotMenu.AddSetting("Enabled", &s_TriggerbotEnabled);
    triggerbotMenu.AddSetting("Range", &s_TriggerbotRange, 0.0f, 20.0f);
    triggerbotMenu.AddSetting("Delay", &s_TriggerbotDelay, 0, 1000);

    SubMenu& recoilControlMenu = hacksMenu.AddSubMenu("Recoil Control");
    recoilControlMenu.AddSetting("Enabled", &s_RecoilControlEnabled);
    recoilControlMenu.AddSetting("Strength", &s_RecoilControlStrength, 0, 100);

    SubMenu& visualsMenu = hacksMenu.AddSubMenu("Visuals");
    visualsMenu.AddSetting("Outlines", &s_OutlinesEnabled);
    visualsMenu.AddSetting("Tracers", &s_TracersEnabled);
    visualsMenu.AddSetting("FOV", &s_FOVVisualEnabled);

    Menu& settingsMenu = gui.AddTab("E", "Settings");
    SubMenu& keybindsMenu = settingsMenu.AddSubMenu("Keybinds");
    SubMenu& arduinoMenu = settingsMenu.AddSubMenu("Arduino");
    int choice = 0;
    std::vector<const char*> items = { "Test 1", "Test 2", "Test 3" };
    arduinoMenu.AddSetting("COM Ports", &choice, &items);

    arduinoMenu.AddButton("Connect", []() {
        std::cout << "test" << std::endl;
    });

    long long lastShot = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    HWINEVENTHOOK hWinEventHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!hWinEventHook) {
        std::cerr << "Error setting up event hook" << std::endl;
        return EXIT_FAILURE;
    }
    
    while (true)
    {
        if (!handle)
            handle = hid_open(0x413c, 0x301a, L"CHIDLC");

        if (!Window::Begin())
            break;
        
        if (Window::IsOpen() && Window::IsEnabled())
        {
            gui.Render();
        }


        // Capture screen data in an array of rgba values
        unsigned char* imageData = screen.CaptureScreen(Globals::s_CaptureLocation.x, Globals::s_CaptureLocation.y);

        // Convert the captured screen data to an opencv image
        cv::Mat image(Globals::s_FOVSize.y, Globals::s_FOVSize.x, CV_8UC4, imageData);

        // Check if the image was successfully loaded
        if (image.empty())
        {
            std::cerr << "Error: Couldn't load image!" << std::endl;
            return EXIT_FAILURE;
        }

        // Calculate the targeted enemy, if there are no enemies just do nothing
        cv::Point centerHead = FindEnemy(image);
        if (centerHead.x != 0 && centerHead.y != 0)
        {
            // If the alt key is pressed, activate aimbot
            if (GetAsyncKeyState(ImGuiKeyToVK(s_AimbotKey.KeyCode)) & 0x8000 && !Window::IsOpen() && Window::IsEnabled())
            {
                // Calculate the location of the enemy relative the the crosshair
                cv::Point target = (centerHead - Globals::s_FOVCenter) / s_AimbotSmoothing;
                if (target.x > 80) target.x = 80;
                if (target.x < -80) target.x = -80;
                if (target.y > 80) target.y = 80;
                if (target.y < -80) target.y = -80;

                double distance = std::sqrt(target.x * target.x + target.y + target.y);
                long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                if (handle && s_TriggerbotEnabled && distance < s_TriggerbotRange && now - lastShot > s_TriggerbotDelay) {
                    lastShot = now;
                    unsigned char buf[3];
                    buf[0] = 0;
                    buf[1] = 'C';
                    buf[2] = '1';
                    hid_write(handle, buf, sizeof(buf));
                }

                if (handle && s_AimbotEnabled) {
                    unsigned char buf[4];
                    buf[0] = 0;
                    buf[1] = 'M';
                    buf[2] = target.x;
                    buf[3] = target.y;
                    hid_write(handle, buf, sizeof(buf));
                }
            }
        }

        // Recoil control
        if (handle && GetAsyncKeyState(VK_LBUTTON) && s_RecoilControlEnabled)
        {
            unsigned char buf[4];
            buf[0] = 0;
            buf[1] = 'M';
            buf[2] = 0;
            buf[3] = -s_RecoilControlStrength;
            hid_write(handle, buf, sizeof(buf));
        }

        if (s_FOVVisualEnabled && Window::IsEnabled())
        {
            ImDrawList* drawList = ImGui::GetBackgroundDrawList();
            cv::Point min = Globals::s_WindowCenter - Globals::s_FOVCenter;
            cv::Point max = Globals::s_WindowCenter + Globals::s_FOVCenter;
            drawList->AddRect({ (float)min.x, (float)min.y }, { (float)max.x, (float)max.y }, IM_COL32(255, 255, 255, 255));
        }

        Window::End();
    }

    UnhookWinEvent(hWinEventHook);
    Window::Shutdown();

    return EXIT_SUCCESS;
}

cv::Point FindEnemy(cv::Mat& image)
{
    // Define the lower and upper bounds for the mask
    static cv::Scalar lowerBound = { 140, 120, 180 };
    static cv::Scalar upperBound = { 160, 200, 255 };

    std::vector<Contour> contours = Detection::FindContours(image, lowerBound, upperBound);

    if (s_OutlinesEnabled && Window::IsEnabled())
    {
        // Render the contours on a new window
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        for (const auto& contour : contours)
        {
            for (size_t i = 0; i < contour.size() - 1; ++i)
            {
                ImVec2 p1 = ImVec2(contour[i].x + Globals::s_WindowSize.x / 2 - Globals::s_FOVCenter.x, contour[i].y + Globals::s_WindowSize.y / 2 - Globals::s_FOVCenter.y);
                ImVec2 p2 = ImVec2(contour[i + 1].x + Globals::s_WindowSize.x / 2 - Globals::s_FOVCenter.x, contour[i + 1].y + Globals::s_WindowSize.y / 2 - Globals::s_FOVCenter.y);

                // Assuming you want to draw lines, modify as needed
                drawList->AddLine(p1, p2, IM_COL32(255, 255, 255, 255), 2.0f);
            }
        }
    }

    Contour contour = Detection::FindBestContour(contours, Globals::s_FOVSize);
    cv::Point top = Detection::FindTop(contour);
    top.y += s_AimbotYOffset;

    if (s_TracersEnabled && (top.x != 0 && top.y != 0) && Window::IsEnabled())
    {
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        drawList->AddLine(ImVec2{ (float)Globals::s_WindowCenter.x, (float)Globals::s_WindowCenter.y }, ImVec2{ (float)top.x + Globals::s_WindowCenter.x - Globals::s_FOVCenter.x, (float)top.y + Globals::s_WindowCenter.y - Globals::s_FOVCenter.y }, IM_COL32(255, 255, 255, 255), 2.0f);
    }

    return top;
}

int ImGuiKeyToVK(ImGuiKey key) {
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

    case ImGuiKey_ModCtrl: return VK_CONTROL;
    case ImGuiKey_ModShift: return VK_SHIFT;
    case ImGuiKey_ModAlt: return VK_MENU;
    case ImGuiKey_ModSuper: return VK_LWIN;
    case ImGuiKey_MouseLeft: return VK_LBUTTON;
    case ImGuiKey_MouseRight: return VK_RBUTTON;
    case ImGuiKey_MouseMiddle: return VK_MBUTTON;
    case ImGuiKey_MouseX1: return VK_XBUTTON1;
    case ImGuiKey_MouseX2: return VK_XBUTTON2;

    default: return 0;
    }
}