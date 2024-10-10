#pragma once

#include "ldpch.h"

#include "Core.h"
#include "Events/Event.h"

namespace Ludo {

	struct WindowProps
	{
		std::string Title;
		unsigned int Width, Height;

		WindowProps(const std::string& title = "Ludo Engine",
			unsigned int width = 1280, unsigned int height = 720)
			: Title(title), Width(width), Height(height) {}
	};

	// Interface used to abstract platform specific window creation code (althought LudoNarrative is windows only)
	class LUDO_API Window
	{
	public:
		using EventCallBackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetEventCallBack(const EventCallBackFn& callback) = 0;
		virtual void SetVsync(bool enbaled) = 0;
		virtual bool IsVsync() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};

}