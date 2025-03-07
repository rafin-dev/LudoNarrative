#pragma once

#ifdef LD_PLATFORM_WINDOWS

#define NOMINMAX
#include <Windows.h>

#ifndef LUDO_DIST

extern Ludo::Application* Ludo::CreateApplication();

int main(int argc, char** argv)
{
	Ludo::Log::Init();

	LD_PROFILE_BEGIN_SESSION("Startup", "LudoNarrative-Profile-Startup.json");
	Ludo::Application* app = Ludo::CreateApplication();
	LD_PROFILE_END_SESSION();
	
	LD_PROFILE_BEGIN_SESSION("Runtime", "LudoNarrative-Profile-Runtime.json");
	app->Run();
	LD_PROFILE_END_SESSION();
	
	LD_PROFILE_BEGIN_SESSION("Shutdown", "LudoNarrative-Profile-Shutdown.json");
	delete app;
	LD_PROFILE_END_SESSION();
}

#else

extern Ludo::Application* Ludo::CreateApplication();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) 
{
	Ludo::Application* app = Ludo::CreateApplication();

	app->Run();

	delete app;
}

#endif

#endif 
