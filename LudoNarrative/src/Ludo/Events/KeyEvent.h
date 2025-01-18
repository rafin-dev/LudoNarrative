#pragma once

#include "Event.h"

#include "Ludo/Core/KeyCodes.h"

namespace Ludo {

	// Currently some key events will have an undefined keycode
	// Don't rely on these as they are platform dependent and thus, undefined behaviour

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

	// Might split this into two events: KeyPressedEvent and KeyHoldEvent
	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(KeyCode keycode, int repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream stringStream;
			stringStream << "KeyPressedEvent: " << (int)m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return stringStream.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)

	private:
		int m_RepeatCount;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(KeyCode keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream stringStream;
			stringStream << "KeyReleasedEvent: " << (int)m_KeyCode;
			return stringStream.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class CharTypedEvent : public Event
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