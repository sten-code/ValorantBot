#include "LayerStack.h"

LayerStack::~LayerStack()
{
    for (Layer* layer : m_Layers) {
        layer->OnDetach();
        delete layer;
    }
}

bool LayerStack::DispatchEvent(Event& e)
{
    for (Layer* layer : m_Layers) {
        layer->OnEvent(e);
        if (e.Handled) {
            return true;
        }
    }
    return false;
}
