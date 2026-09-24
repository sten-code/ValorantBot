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

    float GetStatusBarHeight() const { return m_StatusBarHeight; }
    void SetStatusBarHeight(float height) { m_StatusBarHeight = height; }

private:
    void OnEvent(Event& e);

protected:
    bool m_Running = false;

    std::unique_ptr<Window> m_Window = nullptr;
    LayerStack m_LayerStack;

    float m_StatusBarHeight = 0.0f;
};

Application* CreateApplication();
