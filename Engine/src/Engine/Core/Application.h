#pragma once

#include <Engine/Core/Layers/LayerStack.h>
#include <Engine/Core/Window.h>
#include <Engine/Events/ApplicationEvents.h>

class Application {
public:
    explicit Application(const WindowProps& windowProps);
    virtual ~Application();

    void Run();
    virtual void Exit();

private:
    void OnEvent(Event& e);

protected:
    bool m_Running = false;

    std::unique_ptr<Window> m_Window = nullptr;
    LayerStack m_LayerStack;
};

Application* CreateApplication();
