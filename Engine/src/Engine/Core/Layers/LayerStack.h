#pragma once

#include "Layer.h"

#include <vector>

class LayerStack : public EventDispatcher {
public:
    LayerStack() = default;
    ~LayerStack() override;

    bool DispatchEvent(Event& e) override;

    template <typename T, typename... Args>
    void PushLayer(Args&&... args)
    {
        static_assert(std::is_base_of_v<Layer, T>, "T must derive from Layer");
        T* layer = new T(std::forward<Args>(args)...);
        m_Layers.push_back(layer);
        layer->OnAttach();
    }

    template <typename T>
    void PopLayer()
    {
        static_assert(std::is_base_of_v<Layer, T>, "T must derive from Layer");
        auto it = std::find_if(m_Layers.begin(), m_Layers.end(), [](Layer* layer) {
            return dynamic_cast<T*>(layer) != nullptr;
        });
        if (it != m_Layers.end()) {
            (*it)->OnDetach();
            delete *it;
            m_Layers.erase(it);
        }
    }

    template <typename T>
    T* GetLayer()
    {
        static_assert(std::is_base_of_v<Layer, T>, "T must derive from Layer");
        auto it = std::find_if(m_Layers.begin(), m_Layers.end(), [](Layer* layer) {
            return dynamic_cast<T*>(layer) != nullptr;
        });
        return (it != m_Layers.end()) ? dynamic_cast<T*>(*it) : nullptr;
    }

    [[nodiscard]] std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
    [[nodiscard]] std::vector<Layer*>::iterator end() { return m_Layers.end(); }
    [[nodiscard]] std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
    [[nodiscard]] std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

    [[nodiscard]] std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
    [[nodiscard]] std::vector<Layer*>::const_iterator end() const { return m_Layers.end(); }
    [[nodiscard]] std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
    [[nodiscard]] std::vector<Layer*>::const_reverse_iterator rend() const { return m_Layers.rend(); }

private:
    std::vector<Layer*> m_Layers;
};