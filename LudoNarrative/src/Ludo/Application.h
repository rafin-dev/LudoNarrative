#pragma once

#include "Core.h"
#include "Window.h"

#include "Events/ApplicationEvent.h"

namespace Ludo {

	class LUDO_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(Event& event);

	private:
		bool CloseWindow(WindowCloseEvent& event);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	// To be defined in client
	Application* CreateApplication();
}