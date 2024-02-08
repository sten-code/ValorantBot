#include "gui.h"

#include "imgui_custom.h"

#include <byte_array.h>

ImFont* combo_arrow;
ImFont* bold;
ImFont* tab_icons;
ImFont* tab_title_icon;
ImFont* tab_title;
ImFont* subtab_title;
ImFont* logo;

Gui::Gui()
{
    ImFontConfig font_config;
    font_config.PixelSnapH = false;
    font_config.OversampleH = 5;
    font_config.OversampleV = 5;
    font_config.RasterizerMultiply = 1.2f;

    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
        0xE000, 0xE226, // icons
        0,
    };

    ImGuiIO& io = ImGui::GetIO();
    bold = io.Fonts->AddFontFromMemoryTTF(PTRootUIBold, sizeof(PTRootUIBold), 15.0f, &font_config, ranges);
    tab_icons = io.Fonts->AddFontFromMemoryTTF(clarityfont, sizeof(clarityfont), 15.0f, &font_config, ranges);
    logo = io.Fonts->AddFontFromMemoryTTF(clarityfont, sizeof(clarityfont), 21.0f, &font_config, ranges);
    tab_title = io.Fonts->AddFontFromMemoryTTF(PTRootUIBold, sizeof(PTRootUIBold), 19.0f, &font_config, ranges);
    tab_title_icon = io.Fonts->AddFontFromMemoryTTF(clarityfont, sizeof(clarityfont), 18.0f, &font_config, ranges);
    subtab_title = io.Fonts->AddFontFromMemoryTTF(PTRootUIBold, sizeof(PTRootUIBold), 15.0f, &font_config, ranges);
    combo_arrow = io.Fonts->AddFontFromMemoryTTF(combo, sizeof(combo), 9.0f, &font_config, ranges);
}

void Gui::Render()
{
    Menu* selectedMenu = m_SelectedMenuIndex == -1 ? nullptr : &m_Menus[m_SelectedMenuIndex];
    SubMenu* selectedSubMenu = (selectedMenu == nullptr || selectedMenu->SelectedSubMenuIndex == -1) ? nullptr : &selectedMenu->SubMenus[selectedMenu->SelectedSubMenuIndex];

    ImGui::SetNextWindowSize({ 730, 460 });
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

    ImGui::Begin("GUI", nullptr, ImGuiWindowFlags_NoDecoration); {
        ImDrawList* draw = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetWindowSize();
        ImGuiStyle style = ImGui::GetStyle();

        // Draw dividing lines     
        draw->AddRectFilled(pos, ImVec2(pos.x + 210, pos.y + size.y), ImColor(24, 24, 26), style.WindowRounding, ImDrawFlags_RoundCornersLeft);
        draw->AddLine(ImVec2(pos.x + 210, pos.y + 2), ImVec2(pos.x + 210, pos.y + size.y - 2), ImColor(1.0f, 1.0f, 1.0f, 0.03f));
        draw->AddLine(ImVec2(pos.x + 47, pos.y + 2), ImVec2(pos.x + 47, pos.y + size.y - 2), ImColor(1.0f, 1.0f, 1.0f, 0.03f));
        draw->AddLine(ImVec2(pos.x + 2, pos.y + 47), ImVec2(pos.x + 47, pos.y + 47), ImColor(1.0f, 1.0f, 1.0f, 0.03f));
        draw->AddLine(ImVec2(pos.x + 63, pos.y + 47), ImVec2(pos.x + 195, pos.y + 47), ImColor(1.0f, 1.0f, 1.0f, 0.03f));
        
        // Draw logos
        draw->AddText(logo, 21.0f, ImVec2(pos.x + 14, pos.y + 12), ImColor(147, 190, 66), "A");
        if (selectedMenu != nullptr) 
        {
            draw->AddText(tab_title_icon, 18.0f, ImVec2(pos.x + 65, pos.y + 14), ImColor(147, 190, 66), selectedMenu->Icon.c_str());
            draw->AddText(tab_title, 19.0f, ImVec2(pos.x + 93, pos.y + 15), ImColor(1.0f, 1.0f, 1.0f), selectedMenu->Name.c_str());
        }
        
        draw->AddRect(pos + ImVec2(1, 1), pos + size - ImVec2(1, 1), ImColor(1.0f, 1.0f, 1.0f, 0.03f), style.WindowRounding);

        ImGui::SetCursorPos({ 8, 56 });
        ImGui::BeginGroup();
        for (size_t i = 0; i < m_Menus.size(); i++)
        {
            if (selectedMenu != nullptr && Elements::Tab(m_Menus[i].Icon.c_str(), m_Menus[i].Name == selectedMenu->Name))
                m_SelectedMenuIndex = i;
        }
        ImGui::EndGroup();

        ImGui::SetCursorPos({ 57, 56 });
        ImGui::BeginGroup();
        if (selectedMenu != nullptr && selectedSubMenu != nullptr)
        {
            for (size_t i = 0; i < selectedMenu->SubMenus.size(); i++)
                if (Elements::SubTab(selectedMenu->SubMenus[i].Name.c_str(), selectedMenu->SubMenus[i].Name == selectedSubMenu->Name))
                    selectedMenu->SelectedSubMenuIndex = i;
        }
        ImGui::EndGroup();

        if (selectedSubMenu != nullptr) 
        {
            ImGui::SetCursorPos({ 226, 16 });
            Elements::BeginChild(selectedSubMenu->Name.c_str(), ImVec2(488, 429));
            for (std::unique_ptr<Setting>& setting : selectedSubMenu->Settings)
                setting->Render();
            Elements::EndChild();
        }
    }
    ImGui::End();

    ImGui::PopStyleVar();
}
