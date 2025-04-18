#include "ldpch.h"

#include "WindowsWindow.h"

#include "Ludo/Core/Log.h"
#include "Ludo/Events/ApplicationEvent.h"
#include "Ludo/Events/KeyEvent.h"
#include "Ludo/Events/MouseEvent.h"
#include "Ludo/Core/KeyCodes.h"

#include "backends/imgui_impl_win32.h"

#include "shellapi.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Ludo {

	bool WindowsWindow::s_WindowClassInitialized = false;
	const wchar_t* WindowsWindow::s_ClassName = L"Ludo Window Class";
	WNDCLASS WindowsWindow::s_WindowClass;

	Window* Window::Create(bool* output, const WindowProps& props)
	{
		return new WindowsWindow(props, output);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props, bool* output)
	{
		bool result = Init(props);
		if (output != nullptr)
		{
			*output = result;
		}
	}

	WindowsWindow::~WindowsWindow()
	{
		ShutDown();
	}

	void WindowsWindow::OnUpdate()
	{
		LD_PROFILE_FUNCTION();

		MSG msg;
		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (m_ShouldResize)
		{
			m_ShouldResize = false;
			m_Context->Resize(m_Data.Width, m_Data.Height);
		}

		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVsync(bool enabled)
	{
		m_Data.Vsync = enabled;
	}

	bool WindowsWindow::IsVsync() const
	{
		return m_Data.Vsync;
	}

	void WindowsWindow::SetTitle(std::string& title)
	{
		SetWindowText(m_WindowHandle, std::filesystem::path(title).wstring().c_str());
	}

	void WindowsWindow::SetSize(uint32_t width, uint32_t height)
	{
		RECT windowRect;
		if (GetWindowRect(m_WindowHandle, &windowRect))
		{
			SetWindowPos(m_WindowHandle, nullptr, windowRect.left, windowRect.top, width, height, SWP_NOOWNERZORDER);
		}
	}

	void WindowsWindow::SetFullScreen(bool enabled)
	{
		// Window style
		DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
		DWORD exStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW;
		if (enabled)
		{
			style = WS_POPUP | WS_VISIBLE;
			exStyle = WS_EX_APPWINDOW;
		}

		SetWindowLongW(m_WindowHandle, GWL_STYLE, style);
		SetWindowLongW(m_WindowHandle, GWL_EXSTYLE, exStyle);

		// Window size
		if (enabled)
		{
			GetWindowRect(m_WindowHandle, &m_PreviousRect);

			HMONITOR monitor = MonitorFromWindow(m_WindowHandle, MONITOR_DEFAULTTONEAREST);
			MONITORINFO info = {};
			info.cbSize = sizeof(info);
			if (GetMonitorInfoW(monitor, &info))
			{
				SetWindowPos(m_WindowHandle, nullptr,
					info.rcMonitor.left, info.rcMonitor.top,
					info.rcMonitor.right - info.rcMonitor.left,
					info.rcMonitor.bottom - info.rcMonitor.top,
					SWP_NOZORDER);
			}
		}
		else
		{
			SetWindowPos(m_WindowHandle, nullptr,
				m_PreviousRect.left, m_PreviousRect.top,
				m_PreviousRect.right - m_PreviousRect.left,
				m_PreviousRect.bottom - m_PreviousRect.top,
				SWP_NOZORDER);
			ShowWindow(m_WindowHandle, SW_SHOW);
		}

		m_Data.IsFullScreen = enabled;
	}

	bool WindowsWindow::Init(const WindowProps& props)
	{
		LD_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.Vsync = true;
		
		bool InitImGui = false;
		if (!s_WindowClassInitialized)
		{
			InitializeWinAPI();
			s_WindowClassInitialized = true;
			InitImGui = true;
		}
		
		int wchars_num = MultiByteToWideChar(CP_UTF8, 0, m_Data.Title.c_str(), -1, NULL, 0);
		std::vector<wchar_t> wTitle;
		wTitle.reserve(wchars_num);
		MultiByteToWideChar(CP_UTF8, 0, m_Data.Title.c_str(), -1, wTitle.data(), wchars_num);

		RECT wr = { 0, 0, (LONG)m_Data.Width, (LONG)m_Data.Height };
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

		HWND handle = CreateWindowEx(
			0,                            // Custom Style
			s_ClassName,                  // WindowClass
			wTitle.data(),               // Title
			WS_OVERLAPPEDWINDOW,          // Style
			CW_USEDEFAULT, CW_USEDEFAULT, // Posiition
			wr.right - wr.left,           // Width
			wr.bottom - wr.top,           // Height
			NULL,                         // Parent window
			NULL,                         // Menu
			GetModuleHandle(NULL),        // hInstance
			NULL                          // Adicional data
		);
		if (handle == nullptr)
		{
			LD_CORE_ERROR("Failed to create window");
			return false;
		}
		SetWindowLongPtr(handle, GWLP_USERDATA, (long long)this);

		m_WindowHandle = handle;

		m_Context = GraphicsContext::Create(this);
		if (!m_Context->Init())
		{
			return false;
		}

		GetWindowRect(m_WindowHandle, &m_PreviousRect);

		ShowWindow(m_WindowHandle, SW_NORMAL);

		DragAcceptFiles(m_WindowHandle, TRUE);

		LD_CORE_INFO("Created Window: {0} [{1}, {2}]", m_Data.Title, m_Data.Width, m_Data.Height);

		{
			LD_PROFILE_SCOPE("ImGui_ImplWin32_Init");
			if (InitImGui)
			{
				ImGui_ImplWin32_Init(m_WindowHandle);
			}
		}

		return true;
	}

	void WindowsWindow::ShutDown()
	{
		LD_PROFILE_FUNCTION();

		if (m_Context != nullptr) { delete m_Context; m_Context = nullptr; }
		DestroyWindow(m_WindowHandle);
		LD_CORE_INFO("Closed Window: {0}", m_Data.Title);
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
			WindowsWindow * wnd = ((WindowsWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA));
			if (lParam != 0 && (LOWORD(lParam) != wnd->GetWidth() || HIWORD(lParam) != wnd->GetHeight()))
			{
				wnd->m_ShouldResize = true;
				wnd->m_Data.Width = LOWORD(lParam);
				wnd->m_Data.Height = HIWORD(lParam);
			}
			s_EventCallBack((Event&)(WindowResizeEvent(LOWORD(lParam), HIWORD(lParam))));
			return false;
	} },
		{ WM_CLOSE, ThrowEventAndHandle(WindowCloseEvent()) },
		{ WM_QUIT, ThrowEventAndHandle(WindowCloseEvent()) },

		// Input
		// Keyboard
		{ WM_KEYDOWN, ThrowEvent(KeyPressedEvent((KeyCode)wParam, (int)lParam & 0xFFFF)) },
		{ WM_KEYUP, ThrowEvent(KeyReleasedEvent((KeyCode)wParam)) },
		{ WM_CHAR, ThrowEvent(CharTypedEvent((wchar_t)wParam)) },

		// Mouse
		{ WM_MOUSEMOVE, ThrowEvent(MouseMovedEvent(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) },
		{ WM_MOUSEWHEEL, ThrowEvent(MouseScrolledEvent(0, GET_WHEEL_DELTA_WPARAM(wParam))) },

		{ WM_LBUTTONDOWN, ThrowEvent(MouseButtonPressedEvent(MouseButtonCode::Left)) },
		{ WM_LBUTTONUP, ThrowEvent(MouseButtonReleasedEvent(MouseButtonCode::Left)) },

		{ WM_RBUTTONDOWN, ThrowEvent(MouseButtonPressedEvent(MouseButtonCode::Right)) },
		{ WM_RBUTTONUP, ThrowEvent(MouseButtonReleasedEvent(MouseButtonCode::Right)) },

		{ WM_MBUTTONDOWN, ThrowEvent(MouseButtonPressedEvent(MouseButtonCode::Middle)) },
		{ WM_MBUTTONUP, ThrowEvent(MouseButtonReleasedEvent(MouseButtonCode::Middle)) },

		{ WM_DROPFILES, MSGlambda {
			char buffer[256] = {};
			DragQueryFileA((HDROP)wParam, 0, buffer, 256);
			s_EventCallBack((Event&)FileDroppedEvent(std::filesystem::path(buffer)));
			return false;
		} }
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