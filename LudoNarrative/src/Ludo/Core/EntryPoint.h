#pragma once

#ifdef LD_PLATFORM_WINDOWS

extern Ludo::Application* Ludo::CreateApplication();

int main(int argc, char** argv)
{
	Ludo::Log::Init();

	Ludo::Application* app = Ludo::CreateApplication();
	app->Run();
	delete app;
}

#endif 
