#pragma once

#include "Event.h"

namespace Ludo {

	class LUDO_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(int x, int y)
			: m_MouseX(x), m_MouseY(y) {}

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }

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

	class LUDO_API MouseScrolledEvent : public Event
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

	class LUDO_API MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	protected:
		MouseButtonEvent(int button)
			: m_Button(button) {}

		int m_Button;
	};

	class LUDO_API MouseButtonPressedEvent : public MouseButtonEvent
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

	class LUDO_API MouseButtonReleasedEvent : public MouseButtonEvent
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