#pragma once

#include <cstdint>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <map>

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

extern ImFont* tab_icons;

namespace Elements {
    bool Tab(const char* icon, bool boolean);
    bool SubTab(const char* name, bool boolean);
}
