#pragma once

#include "Event.h"

namespace Ludo {

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(int x, int y)
			: m_MouseX(x), m_MouseY(y) {}

		inline int GetX() const { return m_MouseX; }
		inline int GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream stringStream;
			stringStream << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return stringStream.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		int m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(int offsetX)
			: m_OffsetX(offsetX) {}

		inline int GetXOffset() { return m_OffsetX; }

		std::string ToString() const override
		{
			std::stringstream stringStream;
			stringStream << "MouseScrolledEvent: " << m_OffsetX;
			return stringStream.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		int m_OffsetX;
	};

	enum MouseButton : int
	{
		Left,
		Middle,
		Right
	};

	class MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	protected:
		MouseButtonEvent(int button)
			: m_Button(button) {}

		int m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream stringStream;
			stringStream << "MouseButtonPressedEvent: " << m_Button;
			return stringStream.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream stringStream;
			stringStream << "MouseButtonReleasedEvent: " << m_Button;
			return stringStream.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

}