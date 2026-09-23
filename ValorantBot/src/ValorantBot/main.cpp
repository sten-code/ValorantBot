#include <Engine/Core/Application.h>
#include <Engine/Core/EntryPoint.h>
#include <Engine/Core/Log.h>
#include <ValorantBot/MainLayer.h>
#include <ValorantBot/UI/Fonts.h>

#include <imgui.h>

class ValorantBot final : public Application {
public:
    explicit ValorantBot()
        : Application(WindowProps { "ValorantBot", 1280, 720, false })
    {
        Fonts::LoadFonts();

        INFO("Current Working Directory: {}", std::filesystem::current_path().string());

        m_LayerStack.PushLayer<MainLayer>(*m_Window);
    }
};

Application* CreateApplication()
{
    return new ValorantBot();
}
