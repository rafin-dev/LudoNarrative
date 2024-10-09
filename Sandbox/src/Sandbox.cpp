#include <LudoEngine.h>

#include <iostream>

class Sandbox : public Ludo::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

Ludo::Application* Ludo::CreateApplication()
{

	return new Sandbox();
}