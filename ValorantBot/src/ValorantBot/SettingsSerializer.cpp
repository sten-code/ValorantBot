#include "SettingsSerializer.h"

#include <Engine/Core/Log.h>

#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>

namespace {
    namespace fs = std::filesystem;

    fs::path GetConfigPath()
    {
        return fs::current_path() / "Settings.ini";
    }

    bool ParseValue(const std::string& line, std::string& key, std::string& value)
    {
        const size_t separator = line.find('=');
        if (separator == std::string::npos) {
            return false;
        }
        key = line.substr(0, separator);
        value = line.substr(separator + 1);
        return true;
    }

    std::optional<bool> ToBool(const std::string& value)
    {
        if (value == "0") return false;
        if (value == "1") return true;
        return std::nullopt;
    }

    std::optional<int> ToInt(const std::string& value)
    {
        try {
            return std::stoi(value);
        } catch (...) {
            return std::nullopt;
        }
    }

    std::optional<unsigned short> ToHex(const std::string& value)
    {
        try {
            const unsigned long parsed = std::stoul(value, nullptr, 16);
            if (parsed > 0xFFFF) {
                return std::nullopt;
            }
            return static_cast<unsigned short>(parsed);
        } catch (...) {
            return std::nullopt;
        }
    }

    std::optional<float> ToFloat(const std::string& value)
    {
        try {
            return std::stof(value);
        } catch (...) {
            return std::nullopt;
        }
    }

    void WriteLine(std::ofstream& file, const std::string& key, const std::string& value)
    {
        file << key << '=' << value << '\n';
    }

    void WriteHexLine(std::ofstream& file, const std::string& key, const unsigned short value)
    {
        std::ostringstream ss;
        ss << "0x" << std::hex << value;
        WriteLine(file, key, ss.str());
    }
}

bool SettingsSerializer::Load(Config::Settings& settings)
{
    const fs::path path = GetConfigPath();
    std::ifstream file(path);
    if (!file.is_open()) {
        INFO("Settings file '{}' not found, using defaults.", path.string());
        return false;
    }

    Config::Settings loaded;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::string key, value;
        if (!ParseValue(line, key, value)) {
            continue;
        }

        if (key == "aimbot.enabled") {
            if (auto v = ToBool(value)) loaded.aimbot.enabled = *v;
        } else if (key == "aimbot.smoothing") {
            if (auto v = ToFloat(value)) loaded.aimbot.smoothing = *v;
        } else if (key == "aimbot.yoffset") {
            if (auto v = ToInt(value)) loaded.aimbot.yOffset = *v;
        } else if (key == "aimbot.key") {
            if (auto v = ToInt(value)) loaded.aimbot.key.KeyCode = static_cast<ImGuiKey>(*v);
        }

        else if (key == "triggerbot.enabled") {
            if (auto v = ToBool(value)) loaded.triggerbot.enabled = *v;
        } else if (key == "triggerbot.range") {
            if (auto v = ToFloat(value)) loaded.triggerbot.range = *v;
        } else if (key == "triggerbot.delay") {
            if (auto v = ToInt(value)) loaded.triggerbot.delayMs = *v;
        }

        else if (key == "device.vid") {
            if (auto v = ToHex(value)) loaded.device.vid = *v;
        } else if (key == "device.pid") {
            if (auto v = ToHex(value)) loaded.device.pid = *v;
        } else if (key == "device.serial") {
            if (!value.empty()) loaded.device.serial = value;
        }
    }

    settings = loaded;
    INFO("Loaded settings from '{}'.", path.string());
    return true;
}

bool SettingsSerializer::Save(const Config::Settings& settings)
{
    const fs::path path = GetConfigPath();
    std::ofstream file(path);
    if (!file.is_open()) {
        ERR("Failed to open settings file '{}' for writing.", path.string());
        return false;
    }

    file << "# ValorantBot settings\n";

    WriteLine(file, "aimbot.enabled", settings.aimbot.enabled ? "1" : "0");
    WriteLine(file, "aimbot.smoothing", std::to_string(settings.aimbot.smoothing));
    WriteLine(file, "aimbot.yoffset", std::to_string(settings.aimbot.yOffset));
    WriteLine(file, "aimbot.key", std::to_string(static_cast<int>(settings.aimbot.key.KeyCode)));
    file << '\n';

    WriteLine(file, "triggerbot.enabled", settings.triggerbot.enabled ? "1" : "0");
    WriteLine(file, "triggerbot.range", std::to_string(settings.triggerbot.range));
    WriteLine(file, "triggerbot.delay", std::to_string(settings.triggerbot.delayMs));
    file << '\n';

    WriteHexLine(file, "device.vid", settings.device.vid);
    WriteHexLine(file, "device.pid", settings.device.pid);
    WriteLine(file, "device.serial", settings.device.serial);

    INFO("Saved settings to '{}'.", path.string());
    return true;
}