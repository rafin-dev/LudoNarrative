#pragma once

#ifdef PRPL_PLATFORM_WINDOWS

extern Purple::Application* Purple::CreateApplication();

int main(int argc, char** argv)
{
	Purple::Application* app = Purple::CreateApplication();
	app->Run();
	delete app;
}

#endif 
