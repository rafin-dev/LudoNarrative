#pragma once

#include "Event.h"

#include "Ludo/Core/KeyCodes.h"

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
		MouseScrolledEvent(int offsetX, int offsetY)
			: m_OffsetY(offsetY), m_OffsetX(offsetX) {}

		inline int GetXOffset() { return m_OffsetX; }
		inline int GetYOffset() { return m_OffsetY; }

		std::string ToString() const override
		{
			std::stringstream stringStream;
			stringStream << "MouseScrolledEvent: " << m_OffsetX;
			return stringStream.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		int m_OffsetY;
		int m_OffsetX;
	};

	class MouseButtonEvent : public Event
	{
	public:
		inline MouseButtonCode GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	protected:
		MouseButtonEvent(MouseButtonCode button)
			: m_Button(button) {}

		MouseButtonCode m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(MouseButtonCode button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream stringStream;
			stringStream << "MouseButtonPressedEvent: " << (int)m_Button;
			return stringStream.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(MouseButtonCode button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream stringStream;
			stringStream << "MouseButtonReleasedEvent: " << (int)m_Button;
			return stringStream.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

}