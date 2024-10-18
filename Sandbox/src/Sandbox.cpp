#include <LudoEngine.h>

#include <iostream>

class ExampleLayer : public Ludo::Layer
{
public:
	ExampleLayer()
		: Ludo::Layer("Example")
	{
	}

	void OnUpdate() override
	{
	}

	void OnEvent(Ludo::Event& event) override
	{
	}
};

class Sandbox : public Ludo::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Ludo::ImGuiLayer());
	}

	~Sandbox()
	{

	}
};

Ludo::Application* Ludo::CreateApplication()
{

	return new Sandbox();
}