#include <Engine/Core/Application.h>
#include <Engine/Core/EntryPoint.h>
#include <Engine/Core/Log.h>
#include <ValorantBot/MainLayer.h>
#include <ValorantBot/UI/Fonts.h>
#include <ValorantBot/UI/Theme.h>

#include <imgui.h>

#include <filesystem>

class ValorantBot final : public Application {
public:
    explicit ValorantBot()
        : Application(WindowProps { "ValorantBot", 1280, 720, false })
    {
        Theme::Apply();
        Fonts::LoadFonts();
        SetStatusBarHeight(MainLayer::STATUS_BAR_HEIGHT);

        INFO("Current Working Directory: {}", std::filesystem::current_path().string());

        m_LayerStack.PushLayer<MainLayer>(*m_Window);
    }
};

Application* CreateApplication()
{
    return new ValorantBot();
}