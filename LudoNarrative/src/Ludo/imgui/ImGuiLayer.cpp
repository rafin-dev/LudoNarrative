#include "ldpch.h"
#include "ImGuiLayer.h"

#include <imgui.h>

#include "Ludo/Renderer/InternalRenderer.h"

namespace Ludo {

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	void ImGuiLayer::begin()
	{
		InternalRenderer::Get()->BeginImGui();
	}

	void ImGuiLayer::End()
	{
		InternalRenderer::Get()->EndImGui();
	}

	void ImGuiLayer::OnImGuiRender()
	{
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
	}

}