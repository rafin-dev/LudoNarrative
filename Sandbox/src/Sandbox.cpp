#include <LudoEngine.h>

#include <iostream>

#include "imgui/imgui.h"

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

	void OnImGuiRender() override
	{
		//ImGui::ShowDemoWindow();
	}

};

class Sandbox : public Ludo::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	void OnUpdate()
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