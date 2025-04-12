#pragma once

#include "Core.h"
#include "Window.h"

#include "Ludo/Events/ApplicationEvent.h"
#include "Ludo/Events/KeyEvent.h"
#include "Ludo/Core/TimeStep.h"

namespace Ludo {

	class Application
	{
	public:
		Application(const std::string& name = "LudoNarrative", uint32_t windowWidth = 1280, uint32_t windowHeight = 720);
		virtual ~Application();

		void Run();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(TimeStep time) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		void Close() { m_Running = false; }

		void ImGuiBlockEvent(bool active) { m_ImGuiBlockEvent = active; }

		Window& GetWindow() { return *m_Window; }

		uint64_t GetCurrentFrame() { return m_FrameCounter; }

		static Application& Get() { return *s_Instance; }

	private:
		void OnEventInternal(Event& event);

		bool CloseWindow(WindowCloseEvent& event);
		bool ResizeWindow(WindowResizeEvent& event);

		Window* m_Window;
		bool m_Running = true;
		bool m_Minimized = false;

		float m_LastFrameTime = 0.0f;
		uint64_t m_FrameCounter = 0;

		bool m_ImGuiBlockEvent = true;

		static Application* s_Instance;
	};

	// To be defined in client
	Application* CreateApplication();
}