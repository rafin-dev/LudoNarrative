#pragma once

#include "Event.h"

namespace Ludo {

	// Currently some key events will have an undefined keycode
	// Don't rely on these as they are platform dependent and thus, undefined behaviour

	class LUDO_API KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		KeyEvent(int keycode)
			: m_KeyCode(keycode) {}

		int m_KeyCode;
	};

	// Might split this into two events: KeyPressedEvent and KeyHoldEvent
	class LUDO_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream stringStream;
			stringStream << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return stringStream.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)

	private:
		int m_RepeatCount;
	};

	class LUDO_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream stringStream;
			stringStream << "KeyReleasedEvent: " << m_KeyCode;
			return stringStream.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class LUDO_API CharTypedEvent : public Event
	{
	public:
		CharTypedEvent(wchar_t charTyped)
			: m_Char(charTyped) {}

		std::string ToString() const override
		{
			std::stringstream stringStream;
			stringStream << "CharTypedEvent: CharUnicode: " << (uint16_t)m_Char;
			return stringStream.str();
		}

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard)

		EVENT_CLASS_TYPE(CharTyped)

	private:
		wchar_t m_Char;
	};

}