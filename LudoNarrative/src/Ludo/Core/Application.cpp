#include "ldpch.h"
#include "Application.h"

#include "Ludo/Core/Core.h"

#include "Ludo/Core/Log.h"
#include "Ludo/Renderer/RendererAPI.h"
#include "Ludo/Renderer/Renderer.h"
#include "Ludo/Core/Input.h"

#include "Ludo/Events/ApplicationEvent.h"
#include "Ludo/Events/KeyEvent.h"
#include "Ludo/Events/MouseEvent.h"
#include "Ludo/Events/Event.h"
#include "Ludo/Core/KeyCodes.h"

#include "imgui/imgui.h"

namespace Ludo {

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
		m_Window = Window::Create(&result);
		if (!result)
		{
			m_Running = false;
			return;
		}
		m_Window->SetEventCallBack(LUDO_BIND_EVENT_FN(Application::OnEvent));
		m_Window->SetVsync(true);
	}

	Application::~Application()
	{
		m_LayerStack.Clear();
		delete m_Window;
		Renderer::ShutDown();
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = ImGui::GetTime();
			TimeStep timeStep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
				{
					layer->OnUpdate(timeStep);
				}

				RenderCommand::BeginImGui();
				for (Layer* l : m_LayerStack)
				{
					l->OnImGuiRender();
				}
				RenderCommand::EndImGui();
			}

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(LUDO_BIND_EVENT_FN(Application::CloseWindow));
		dispatcher.Dispatch<WindowResizeEvent>(LUDO_BIND_EVENT_FN(Application::ResizeWindow));
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

	bool Application::ResizeWindow(WindowResizeEvent& event)
	{
		m_Minimized = event.GetWidth() == 0 || event.GetHeight() == 0;

		return false;
	}

}