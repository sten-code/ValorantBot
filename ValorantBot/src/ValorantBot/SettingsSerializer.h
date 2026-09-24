#pragma once

#include <ValorantBot/Settings.h>

namespace SettingsSerializer {
    // Reads/creates a "Settings.ini" file next to the working directory.
    bool Load(Config::Settings& settings);
    bool Save(const Config::Settings& settings);
}