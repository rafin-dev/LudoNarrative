#include <PurpleEngine.h>

#include <iostream>

class Sandbox : public Purple::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

Purple::Application* Purple::CreateApplication()
{
	return new Sandbox();
}