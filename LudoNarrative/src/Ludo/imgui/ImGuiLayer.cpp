#include "ldpch.h"
#include "ImGuiLayer.h"

#include <imgui.h>

namespace Ludo {

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	void ImGuiLayer::OnUpdate()
	{
		ImGui::Begin("Test");
		ImGui::Text("Lorem ipsum dolor sit amet");
		ImGui::Button("Foo");
		ImGui::Button("Bar");
		ImGui::End();
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
	}

}