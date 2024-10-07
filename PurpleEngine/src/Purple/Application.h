#pragma once

#include "Core.h"

namespace Purple {

	class PURPLE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in client
	Application* CreateApplication();

}