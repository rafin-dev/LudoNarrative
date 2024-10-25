#include "ldpch.h"

#include "WindowsWindow.h"

#include "Ludo/Log.h"
#include "Ludo/Events/ApplicationEvent.h"
#include "Ludo/Events/KeyEvent.h"
#include "Ludo/Events/MouseEvent.h"

#include "Platform/DirectX9/imgui/imgui_impl_dx9.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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
		MSG msg;
		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
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

		LD_CORE_TRACE("Creating Window: {0} [{1}, {2}]", m_Data.Title, m_Data.Width, m_Data.Height);

		if (!s_WindowClassInitialized)
		{
			InitializeWinAPI();
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
		SetWindowLongPtr(handle, GWLP_USERDATA, (long long)this);

		m_WindowHandle = handle;

		ShowWindow(m_WindowHandle, SW_NORMAL);
	}

	void WindowsWindow::ShutDown()
	{
		DestroyWindow(m_WindowHandle);
	}

#define MSGlambda [] (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> bool
#define ThrowEvent(x) MSGlambda { s_EventCallBack((Event&)(x)); return false; }
#define ThrowEventAndHandle(x) MSGlambda { s_EventCallBack((Event&)(x)); return true; }

	Window::EventCallBackFn WindowsWindow::s_EventCallBack = [](Event&) {};
	// Map of functions that process the MSG into LudoNarrative events
	// Returns true if it has handled everything and false if it should it be sent to DefWindowProc
	std::unordered_map<UINT, WindowsWindow::WinMsgCallBackFn> WindowsWindow::s_MsgCallBacks
	{
		// Window
		{ WM_SIZE, MSGlambda {	
			s_EventCallBack((Event&)(WindowResizeEvent(LOWORD(lParam), HIWORD(lParam))));
			WindowsWindow* wnd = ((WindowsWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA));
			wnd->m_Data.Width = LOWORD(lParam);
			wnd->m_Data.Height = HIWORD(lParam);
			return false; 
	} },
		{ WM_CLOSE, ThrowEventAndHandle(WindowCloseEvent()) },
		{ WM_QUIT, ThrowEventAndHandle(WindowCloseEvent()) },

		// Input
		// Keyboard
		{ WM_KEYDOWN, ThrowEvent(KeyPressedEvent(wParam, lParam & 0xFFFF)) },
		{ WM_KEYUP, ThrowEvent(KeyReleasedEvent(wParam)) },
		{ WM_CHAR, ThrowEvent(CharTypedEvent(wParam)) },

		// Mouse
		{ WM_MOUSEMOVE, ThrowEvent(MouseMovedEvent(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) },
		{ WM_MOUSEWHEEL, ThrowEvent(MouseScrolledEvent(GET_WHEEL_DELTA_WPARAM(wParam))) },

		{ WM_LBUTTONDOWN, ThrowEvent(MouseButtonPressedEvent(MouseButton::Left)) },
		{ WM_LBUTTONUP, ThrowEvent(MouseButtonReleasedEvent(MouseButton::Left)) },

		{ WM_RBUTTONDOWN, ThrowEvent(MouseButtonPressedEvent(MouseButton::Right)) },
		{ WM_RBUTTONUP, ThrowEvent(MouseButtonReleasedEvent(MouseButton::Right)) },

		{ WM_MBUTTONDOWN, ThrowEvent(MouseButtonPressedEvent(MouseButton::Middle)) },
		{ WM_MBUTTONUP, ThrowEvent(MouseButtonReleasedEvent(MouseButton::Middle)) }
	};

	void WindowsWindow::InitializeWinAPI()
	{
		s_WindowClass = {};

		s_WindowClass.lpfnWndProc = WindowsWindow::WindowProc;
		s_WindowClass.hInstance = GetModuleHandle(NULL);
		s_WindowClass.lpszClassName = s_ClassName;

		RegisterClass(&s_WindowClass);


	}

	LRESULT WindowsWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
		{
			return true;
		}

		auto ite = s_MsgCallBacks.find(uMsg);

		if (ite != s_MsgCallBacks.end())
		{
			if (ite->second(hwnd, uMsg, wParam, lParam))
			{
				return 0;
			}
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

}