#pragma once

#include <Engine/Events/Event.h>

#include <string>

class Layer {
public:
    explicit Layer(const std::string& name = "Layer");
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(float deltaTime) {}

#ifdef ENGINE_IMGUI
    virtual void OnImGuiRender() {}
    virtual void OnImGuiRenderDock() {}
#endif

    virtual void OnEvent(Event& event) {}

    const std::string& GetName() const { return m_DebugName; }

protected:
    std::string m_DebugName;
};
