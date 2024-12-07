#include "Sandbox2D.h"

#include <imgui/imgui.h>

Sandbox2D::Sandbox2D()
	: m_CameraController(1280.0f / 720.0f, true)
{
}

void Sandbox2D::OnAttach()
{

}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnUpdate(Ludo::TimeStep timeStep)
{
	// ========== Update =========
	m_CameraController.OnUpdate(timeStep);

	// ========== Render =========
	Ludo::Renderer2D::BeginScene(m_CameraController.GetCamera());

	Ludo::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f });

	Ludo::Renderer2D::EndScene();
}

void Sandbox2D::OnEvent(Ludo::Event& event)
{
	m_CameraController.OnEvent(event);
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");

	ImGui::End();
}
