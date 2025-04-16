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

#include "Ludo/Assets/AssetManager.h"

#include <imgui.h>

namespace Ludo {

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name, uint32_t windowWidth, uint32_t windowHeight)
	{
		LD_PROFILE_FUNCTION();

		LD_CORE_ASSERT(s_Instance == nullptr, "Application was already initialized");
		s_Instance = this;

		// Init Renderer
		{
			if (!Renderer::Init())
			{
				m_Running = false;
				return;
			}
		}

		// Create Window
		{
			bool result;
			m_Window = Window::Create(&result, WindowProps(name, windowWidth, windowHeight));
			if (!result)
			{
				m_Running = false;
				return;
			}
		}

		m_Window->SetEventCallBack(LUDO_BIND_EVENT_FN(Application::OnEventInternal));
		m_Window->SetVsync(true);

		AssetManager::InitRuntime();
	}

	Application::~Application()
	{
		LD_PROFILE_FUNCTION();

		AssetManager::Shutdown();

		delete m_Window;
		Renderer::ShutDown();
	}

	void Application::Run()
	{
		LD_PROFILE_FUNCTION();

		this->OnAttach();

		while (m_Running)
		{
			LD_PROFILE_SCOPE("Run Loop Iteration");

			float time = (float)ImGui::GetTime();
			TimeStep timeStep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				this->OnUpdate(timeStep);

				RenderCommand::BeginImGui();
				this->OnImGuiRender();
				RenderCommand::EndImGui();
			}

			m_Window->OnUpdate();

			m_FrameCounter++;
		}

		this->OnDetach();
	}

	void Application::OnEventInternal(Event& event)
	{
		LD_PROFILE_FUNCTION();

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(LUDO_BIND_EVENT_FN(Application::CloseWindow));
		dispatcher.Dispatch<WindowResizeEvent>(LUDO_BIND_EVENT_FN(Application::ResizeWindow));

		if (m_ImGuiBlockEvent)
		{
			ImGuiIO& io = ImGui::GetIO();
			event.Handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event.Handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}

		this->OnEvent(event);
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