#pragma once
#include "Event.h"
#include <Engine/Core/MouseCodes.h>

#include <sstream>
#include <string>

class MouseMovedEvent final : public Event
{
public:
	MouseMovedEvent(float x, float y)
		: m_MouseX(x), m_MouseY(y) {
	}

	[[nodiscard]] float GetX() const { return m_MouseX; }
	[[nodiscard]] float GetY() const { return m_MouseY; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseMoved)
	EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
private:
	float m_MouseX, m_MouseY;
};

class MouseScrolledEvent final : public Event
{
public:
	MouseScrolledEvent(float xOffset, float yOffset)
		: m_XOffset(xOffset)
		, m_YOffset(yOffset) {}

	[[nodiscard]] float GetXOffset() const { return m_XOffset; }
	[[nodiscard]] float GetYOffset() const { return m_YOffset; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "MouseScrolledEvent: " << m_XOffset << ", " << m_YOffset;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseScrolled)
	EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
private:
	float m_XOffset, m_YOffset;
};

class MouseButtonEvent : public Event
{
public:
	MouseCode GetMouseButton() const { return m_Button; }

	EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
protected:
	explicit MouseButtonEvent(const MouseCode button)
		: m_Button(button) {}

	MouseCode m_Button;
};

class MouseButtonPressedEvent final : public MouseButtonEvent
{
public:
	explicit MouseButtonPressedEvent(const MouseCode button)
		: MouseButtonEvent(button) {}

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "MouseButtonPressedEvent: " << m_Button;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseButtonPressed)
};

class MouseButtonReleasedEvent final : public MouseButtonEvent
{
public:
	explicit MouseButtonReleasedEvent(const MouseCode button)
		: MouseButtonEvent(button) {}

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "MouseButtonReleasedEvent: " << m_Button;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseButtonReleased)
};