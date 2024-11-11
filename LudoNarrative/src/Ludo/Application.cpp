#include "ldpch.h"
#include "Application.h"

#include "Core.h"

#include "Log.h"
#include "Renderer/RendererAPI.h"
#include "Renderer/Renderer.h"
#include "Input.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/Event.h"
#include "KeyCodes.h"

namespace Ludo {

#define BindFuncFn(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		LD_CORE_ASSERT(s_Instance == nullptr, "Application was already initialized");
		s_Instance = this;

		if (!Renderer::Init())
		{
			m_Running = false;
			return;
		}

		bool result;
		m_Window.reset(Window::Create(&result));
		if (!result)
		{
			m_Running = false;
			return;
		}
		m_Window->SetEventCallBack(BindFuncFn(OnEvent));
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Running)
		{

			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate();
			}

			RenderCommand::BeginImGui();
			for (Layer* l : m_LayerStack)
			{
				l->OnImGuiRender();
			}
			RenderCommand::EndImGui();

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(BindFuncFn(CloseWindow));
		dispatcher.Dispatch<KeyPressedEvent>([](KeyPressedEvent& event) -> bool {

			if (event.GetKeyCode() == LD_KEY_F11)
			{
				Application::Get().GetWindow().SetFullScreen(!Application::Get().GetWindow().IsFullScreen());
			}

			return false;
			});

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