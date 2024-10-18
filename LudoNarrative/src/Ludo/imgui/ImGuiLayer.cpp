#include "ldpch.h"
#include "ImGuiLayer.h"

#include "Platform/DirectX9/imgui/imgui_impl_dx9.h"
#include "Platform/DirectX9/imgui/imgui_impl_win32.h"

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