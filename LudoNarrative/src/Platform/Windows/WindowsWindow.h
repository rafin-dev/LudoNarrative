#pragma once

#include "ldpch.h"

#include "Ludo/Core.h"
#include "Ludo/Window.h"

#include "Ludo/Renderer/GraphicsContext.h"

namespace Ludo {

	class WindowsWindow : public Window
	{
	public:
		using WinMsgCallBackFn = std::function<bool(HWND, UINT, WPARAM, LPARAM)>;

		WindowsWindow(const WindowProps& props, bool* output);
		~WindowsWindow() override;

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		inline bool IsFullScreen() const override { return m_Data.IsFullScreen; }

		inline void SetEventCallBack(const EventCallBackFn& callback) override { s_EventCallBack = callback; }
		void SetVsync(bool enabled) override;
		bool IsVsync() const override;

		void SetFullScreen(bool enabled) override;

		HWND GetHandle() const { return m_WindowHandle; }

	private:
		bool Init(const WindowProps& props);
		void ShutDown();
		void Resize();

		HWND m_WindowHandle;
		GraphicsContext* m_Context = nullptr;

		bool m_ShouldResize = false;
		
		// Holdos the size and position a window had before being turned into fullscreen
		RECT m_PreviousRect = {};

		struct WindowData
		{
			std::string Title = " ";
			unsigned int Width, Height = 0;
			bool IsFullScreen = false;
			bool Vsync = false;
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