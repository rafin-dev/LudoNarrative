#include "ldpch.h"
#include "Application.h"

#include "Log.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/Event.h"

#include "Renderer/InternalRenderer.h"

namespace Ludo {

#define BindFuncFn(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance;

	Application::Application()
	{
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallBack(BindFuncFn(OnEvent));

		InternalRenderer::Get().Init();
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		auto& Rend = InternalRenderer::Get();

		while (m_Running)
		{
			m_Window->OnUpdate();

			Rend.BeginScene();
			
			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate();
			}

			Rend.EndScene();
		}
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(BindFuncFn(CloseWindow));

		for (Layer* layer : m_LayerStack)
		{
			layer->OnEvent(event);

			if (event.Handled)
			{
				break;
			}
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
	}

	bool Application::CloseWindow(WindowCloseEvent& event)
	{
		m_Running = false;
		return true;
	}

}