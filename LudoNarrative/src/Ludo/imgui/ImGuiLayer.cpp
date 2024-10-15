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
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Text("Hi mom, can you understand this?");

		ImGui::EndFrame();
		ImGui::Render();
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
	}

}