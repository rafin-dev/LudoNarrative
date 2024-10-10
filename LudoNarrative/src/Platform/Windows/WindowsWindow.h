#pragma once

#include "ldpch.h"

#include "Ludo/Core.h"
#include "Ludo/Window.h"

namespace Ludo {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		inline void SetEventCallBack(const EventCallBackFn& callback) override { m_Data.EventCallBack = callback; }
		void SetVsync(bool enabled) override;
		bool IsVsync() const override;

	private:
		virtual void Init(const WindowProps& props);
		virtual void ShutDown();

		HWND m_WindowHandle;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool Vsync;

			EventCallBackFn EventCallBack;
		};

		WindowData m_Data;

		static bool s_WindowClassInitialized;
		static const wchar_t* s_ClassName;
		static WNDCLASS s_WindowClass;

		static void InitializeWindowClass();
	};

}