#pragma once

#include "Event.h"

namespace Ludo {

	class 
		WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		// I was tempted to use <const unsigned int& Width = m_Width> for the syntax sugar, but I dont want the memory overhead
		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream stringStream;
			stringStream << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return stringStream.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
		
	private:
		unsigned int m_Width, m_Height;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class FileDroppedEvent : public Event
	{
	public:
		FileDroppedEvent(const std::filesystem::path& path)
			: m_Path(path) { }

		const std::filesystem::path& GetPath() { return m_Path; }

		EVENT_CLASS_TYPE(FileDropped)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		std::filesystem::path m_Path;
	};

}