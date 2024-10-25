#pragma once

#include "ldpch.h"

#include "Ludo/Core.h"
#include "Ludo/Window.h"

namespace Ludo {

	class WindowsWindow : public Window
	{
	public:
		using WinMsgCallBackFn = std::function<bool(HWND, UINT, WPARAM, LPARAM)>;

		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { RECT rect; GetWindowRect(m_WindowHandle, &rect); return rect.right - rect.left; }
		inline unsigned int GetHeight() const override { RECT rect; GetWindowRect(m_WindowHandle, &rect); return rect.bottom - rect.top; }

		inline void SetEventCallBack(const EventCallBackFn& callback) override { s_EventCallBack = callback; }
		void SetVsync(bool enabled) override;
		bool IsVsync() const override;

		HWND GetHandle() { return m_WindowHandle; }

	private:
		virtual void Init(const WindowProps& props);
		virtual void ShutDown();

		HWND m_WindowHandle;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool Vsync;
		};

		WindowData m_Data;

		static EventCallBackFn s_EventCallBack;

		static bool s_WindowClassInitialized;
		static const wchar_t* s_ClassName;
		static WNDCLASS s_WindowClass;

		static std::unordered_map<UINT, WinMsgCallBackFn> s_MsgCallBacks;
		static void InitializeWinAPI();

		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};

}