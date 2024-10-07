#include <PurpleEngine.h>

#include <iostream>

class Sandbox : public Purple::Application
{
public:
	Sandbox()
	{
		std::cout << "It works!" << std::endl;
	}

	~Sandbox()
	{

	}
};

Purple::Application* Purple::CreateApplication()
{
	return new Sandbox();
}