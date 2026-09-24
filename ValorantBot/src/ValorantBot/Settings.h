#pragma once

#include <imgui.h>

#include <string>

struct Hotkey {
    ImGuiKey KeyCode = ImGuiKey_None;
};

namespace Config {
    struct Aimbot {
        bool enabled = true;
        float smoothing = 2.0f;
        int yOffset = 15;
        Hotkey key { ImGuiMod_Alt };
    };

    struct Triggerbot {
        bool enabled = true;
        float range = 5.0f;
        int delayMs = 200;
    };

    struct Device {
        uint16_t vid = 0x413c;
        uint16_t pid = 0x301a;
        std::string serial = "CHIDLC";
    };

    struct Settings {
        Aimbot aimbot;
        Triggerbot triggerbot;
        Device device;

        void Reset();
    };
}