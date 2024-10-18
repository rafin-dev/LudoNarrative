#include "ldpch.h"
#include "Application.h"

#include "Core.h"

#include "Log.h"
#include "Renderer/InternalRenderer.h"
#include "Input.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/Event.h"

namespace Ludo {

#define BindFuncFn(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	// All references to "InternalRenderer" are temporary

	Application::Application()
	{
		LD_CORE_ASSERT(s_Instance == nullptr, "Application was already initialized");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallBack(BindFuncFn(OnEvent));

		InternalRenderer::Get()->Init();
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		auto Rend = InternalRenderer::Get();

		while (m_Running)
		{
			m_Window->OnUpdate();

			Rend->BeginScene();

			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate();
			}

			Rend->EndScene();
		}
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(BindFuncFn(CloseWindow));
		dispatcher.Dispatch<WindowResizeEvent>(BindFuncFn(ResizeWindow));

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

	bool Application::ResizeWindow(WindowResizeEvent& event)
	{
		InternalRenderer::Get()->Resize(event.GetWidth(), event.GetHeight());
		return false;
	}

}