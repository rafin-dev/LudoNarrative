#pragma once

#ifdef LD_PLATFORM_WINDOWS

extern Ludo::Application* Ludo::CreateApplication();

int main(int argc, char** argv)
{
	Ludo::Log::Init();
	LD_CORE_TRACE("Initialized");
	LD_TRACE("Initialized");

	Ludo::Application* app = Ludo::CreateApplication();
	app->Run();
	delete app;
}

#endif 
