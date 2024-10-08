#pragma once

#ifdef PRPL_PLATFORM_WINDOWS

extern Purple::Application* Purple::CreateApplication();

int main(int argc, char** argv)
{
	Purple::Log::Init();
	PRPL_CORE_TRACE("Initialized");
	PRPL_TRACE("Initialized");

	Purple::Application* app = Purple::CreateApplication();
	app->Run();
	delete app;
}

#endif 
