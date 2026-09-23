#pragma once
#include "Event.h"
#include <Engine/Core/KeyCodes.h>

#include <cstdint>
#include <sstream>
#include <string>

class KeyEvent : public Event
{
public:
    inline KeyCode GetKeyCode() const { return m_KeyCode; }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

protected:
    KeyEvent(KeyCode keycode)
        : m_KeyCode(keycode) {}

    KeyCode m_KeyCode;
};

class KeyPressedEvent final : public KeyEvent
{
public:
    KeyPressedEvent(const KeyCode keycode, uint32_t repeatCount)
        : KeyEvent(keycode), m_RepeatCount(repeatCount) {}

    inline uint32_t GetRepeatCount() const { return m_RepeatCount; }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyPressed)

private:
    uint32_t m_RepeatCount;
};

class KeyReleasedEvent final : public KeyEvent
{
public:
    KeyReleasedEvent(const KeyCode keycode)
        : KeyEvent(keycode) {}

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyReleasedEvent: " << m_KeyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyReleased)
};

class KeyTypedEvent final : public KeyEvent
{
public:
    KeyTypedEvent(const KeyCode keycode)
        : KeyEvent(keycode) {}

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyTypedEvent: " << m_KeyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyTyped)
};