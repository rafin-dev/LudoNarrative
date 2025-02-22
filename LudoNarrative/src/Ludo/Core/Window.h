#pragma once

#include "ldpch.h"

#include "Ludo/Core/Core.h"
#include "Ludo/Events/Event.h"

namespace Ludo {

	struct WindowProps
	{
		std::string Title;
		uint32_t Width, Height;

		WindowProps(const std::string& title = "Ludo Engine",
			uint32_t width = 1280, uint32_t height = 720)
			: Title(title), Width(width), Height(height) {}
	};

	// Interface used to abstract platform specific window creation code (althought LudoNarrative is windows only)
	class Window
	{
	public:
		using EventCallBackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual std::string GetTitle() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual bool IsFullScreen() const = 0;

		virtual void SetEventCallBack(const EventCallBackFn& callback) = 0;
		virtual void SetVsync(bool enbaled) = 0;
		virtual bool IsVsync() const = 0;

		virtual void SetTitle(std::string& title) = 0;

		virtual void SetFullScreen(bool enabled) = 0;

		static Window* Create(bool* ouput = nullptr, const WindowProps& props = WindowProps());
	};

}