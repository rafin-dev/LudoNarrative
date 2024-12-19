#include "Sandbox2D.h"

#include <imgui/imgui.h>

Sandbox2D::Sandbox2D()
	: m_CameraController(1280.0f / 720.0f, true)
{
}

void Sandbox2D::OnAttach()
{
	m_Texture = Ludo::Texture2D::Create("assets/textures/ChernoLogo.png");
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

	Ludo::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.0f }, m_Size, m_Rotation, m_Color);
	Ludo::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.0f }, m_Size, m_Rotation, m_Texture);

	Ludo::Renderer2D::EndScene();
}

void Sandbox2D::OnEvent(Ludo::Event& event)
{
	m_CameraController.OnEvent(event);
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Square");

	ImGui::DragFloat2("Position", (float*)&m_Position, 0.1f, 1.0f);
	ImGui::DragFloat2("Size", (float*)&m_Size, 0.1f);
	ImGui::DragFloat("Rotation", &m_Rotation);
	ImGui::ColorEdit4("Color", (float*)&m_Color);

	ImGui::End();
}
