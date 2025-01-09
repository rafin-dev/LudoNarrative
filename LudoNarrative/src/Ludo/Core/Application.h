#pragma once

#include "Core.h"
#include "Window.h"

#include "Ludo/Events/ApplicationEvent.h"
#include "Ludo/Events/KeyEvent.h"
#include "Ludo/Core/Layer.h"
#include "Ludo/Core/LayerStack.h"
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

		void Close() { m_Running = false; }

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		Window& GetWindow() { return *m_Window; }

		uint64_t GetCurrentFrame() { return m_FrameCounter; }

		static Application& Get() { return *s_Instance; }

	private:
		bool CloseWindow(WindowCloseEvent& event);
		bool ResizeWindow(WindowResizeEvent& event);

		Window* m_Window;
		bool m_Running = true;
		bool m_Minimized = false;

		LayerStack m_LayerStack;

		float m_LastFrameTime = 0.0f;
		uint64_t m_FrameCounter = 0;

		static Application* s_Instance;
	};

	// To be defined in client
	Application* CreateApplication();
}