#include "ldpch.h"

#include "WindowsWindow.h"

#include "Ludo/Log.h"

namespace Ludo {

	bool WindowsWindow::s_WindowClassInitialized = false;
	const wchar_t* WindowsWindow::s_ClassName = L"Ludo Window Class";
	WNDCLASS WindowsWindow::s_WindowClass;

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		ShutDown();
	}

	void WindowsWindow::OnUpdate()
	{
	}

	void WindowsWindow::SetVsync(bool enabled)
	{
	}

	bool WindowsWindow::IsVsync() const
	{
		return m_Data.Vsync;
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.Vsync = false;

		LD_CORE_INFO("Creating Window: {0} [{1}, {2}]", m_Data.Title, m_Data.Width, m_Data.Height);

		if (!s_WindowClassInitialized)
		{
			InitializeWindowClass();
			s_WindowClassInitialized = true;
		}
		
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wTitle = converter.from_bytes(m_Data.Title);

		HWND handle = CreateWindowEx(
			0,                            // Custom Style
			s_ClassName,                  // WindowClass
			wTitle.c_str(),               // Title
			WS_OVERLAPPEDWINDOW,          // Style
			CW_USEDEFAULT, CW_USEDEFAULT, // Posiition
			m_Data.Width, m_Data.Height,  // Size
			NULL,                         // Parent window
			NULL,                         // Menu
			GetModuleHandle(NULL),        // hInstance
			NULL                          // Adicional data
		);

		LD_CORE_ASSERT(handle != NULL, "FAILED TO CREATE WINDOW");

		m_WindowHandle = handle;

		ShowWindow(m_WindowHandle, SW_NORMAL);
	}

	void WindowsWindow::ShutDown()
	{
		DestroyWindow(m_WindowHandle);
	}

	void WindowsWindow::InitializeWindowClass()
	{
		s_WindowClass = {};

		// TODO: Implement custom window procedure
		s_WindowClass.lpfnWndProc = DefWindowProc;
		s_WindowClass.hInstance = GetModuleHandle(NULL);
		s_WindowClass.lpszClassName = s_ClassName;

		RegisterClass(&s_WindowClass);
	}

}