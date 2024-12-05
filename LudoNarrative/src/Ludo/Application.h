#pragma once

#include "Core.h"
#include "Window.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Layer.h"
#include "LayerStack.h"
#include "Ludo/Core/TimeStep.h"

namespace Ludo {

	class 
		Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(Event& event);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		Window& GetWindow() { return *m_Window; }

		static Application& Get() { return *s_Instance; }

	private:
		bool CloseWindow(WindowCloseEvent& event);
		bool ResizeWindow(WindowResizeEvent& event);

		Window* m_Window;
		bool m_Running = true;
		bool m_Minimized = false;

		LayerStack m_LayerStack;

		float m_LastFrameTime = 0.0f;

		static Application* s_Instance;
	};

	// To be defined in client
	Application* CreateApplication();
}