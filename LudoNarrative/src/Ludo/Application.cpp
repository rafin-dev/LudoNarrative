#include "ldpch.h"
#include "Application.h"

#include "Log.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/Event.h"

namespace Ludo {

#define BindFuncFn(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallBack(BindFuncFn(OnEvent));
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Running)
		{
			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(BindFuncFn(CloseWindow));

		LD_CORE_TRACE(event.ToString());
	}

	bool Application::CloseWindow(WindowCloseEvent& event)
	{
		m_Running = false;
		return true;
	}

}