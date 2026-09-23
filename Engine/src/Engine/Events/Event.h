#pragma once

#include <Engine/Core/Core.h>

#include <cstdint>
#include <ostream>
#include <string>

enum class EventType {
    None = 0,
    WindowClose,
    WindowResize,
    WindowFocus,
    WindowLostFocus,
    WindowMoved,
    AppTick,
    AppUpdate,
    AppRender,
    KeyPressed,
    KeyReleased,
    KeyTyped,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled
};

enum EventCategory {
    None = 0,
    EventCategoryApplication = BIT(0),
    EventCategoryInput = BIT(1),
    EventCategoryKeyboard = BIT(2),
    EventCategoryMouse = BIT(3),
    EventCategoryMouseButton = BIT(4)
};

#define EVENT_CLASS_TYPE(type)                                                  \
    static EventType GetStaticType() { return EventType::type; }                \
    virtual EventType GetEventType() const override { return GetStaticType(); } \
    virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
    virtual uint32_t GetCategoryFlags() const override { return category; }

class Event {
public:
    virtual ~Event() = default;

    bool Handled = false;

    [[nodiscard]] virtual EventType GetEventType() const = 0;
    [[nodiscard]] virtual const char* GetName() const = 0;
    [[nodiscard]] virtual uint32_t GetCategoryFlags() const = 0;
    [[nodiscard]] virtual std::string ToString() const { return GetName(); }

    [[nodiscard]] bool IsInCategory(EventCategory category) const
    {
        return GetCategoryFlags() & category;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Event& e)
{
    return os << e.ToString();
}

class EventDispatcher {
public:
    virtual ~EventDispatcher() = default;

    virtual bool DispatchEvent(Event& event) = 0;
};
