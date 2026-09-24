#pragma once

#include <imgui.h>

namespace Theme {
    struct Palette {
        ImVec4 Background;
        ImVec4 Panel;
        ImVec4 Surface;
        ImVec4 Border;
        ImVec4 Hover;
        ImVec4 Active;
        ImVec4 Text;
        ImVec4 TextMuted;
        ImVec4 Primary;
        ImVec4 PrimaryDim;
        ImVec4 Success;
        ImVec4 Danger;
    };

    void Apply();

    const Palette& Colors();
}