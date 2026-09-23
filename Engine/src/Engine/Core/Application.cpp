#include "Application.h"
#include <Engine/Core/Log.h>

#ifdef ENGINE_IMGUI
#include "imgui.h"
#endif

Application::Application(const WindowProps& windowProps)
    : m_Window(Window::Create(windowProps))
{
    INFO("Setting event callback");
    m_Window->SetEventCallback(BIND_FN(OnEvent));

#ifdef ENGINE_IMGUI
    INFO("Initializing ImGui");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

    ImGui::StyleColorsDark();

    m_Window->InitializeImGui();
#endif
}

Application::~Application()
{
    m_Window.reset();
#ifdef ENGINE_IMGUI
    ImGui::DestroyContext();
#endif
}

void Application::Run()
{
    m_Running = true;
    while (m_Running) {
        m_Window->Poll();

        for (Layer* layer : m_LayerStack) {
            layer->OnUpdate(0.0f);
        }

#ifdef ENGINE_IMGUI
        m_Window->ImGuiBegin();

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
            m_Window->IsMaximized() ? ImVec2 { 0.0f, 0.0f } : ImVec2 { 0.0f, 0.0f });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4 { 0.0f, 0.0f, 0.0f, 0.0f });

        ImGui::Begin("MainWindow", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking);

        ImGui::PopStyleColor();
        ImGui::PopStyleVar(3);

        for (Layer* layer : m_LayerStack) {
            layer->OnImGuiRender();
        }

        const ImGuiID dockspaceId = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2 { 0.0f, ImGui::GetContentRegionAvail().y });

        for (Layer* layer : m_LayerStack) {
            layer->OnImGuiRenderDock();
        }

        ImGui::End();

        m_Window->ImGuiEnd();
#endif

        m_Window->Render();
    }
}

void Application::Exit()
{
    m_Running = false;
}

void Application::OnEvent(Event& e)
{
    if (m_LayerStack.DispatchEvent(e)) {
        return;
    }

    if (e.GetEventType() == EventType::WindowClose) {
        Exit();
    }
}
