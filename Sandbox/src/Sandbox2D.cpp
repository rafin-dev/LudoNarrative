#include "Sandbox2D.h"

#include <imgui/imgui.h>

Sandbox2D::Sandbox2D()
	: m_CameraController(1280.0f / 720.0f, true)
{
}

void Sandbox2D::OnAttach()
{
	LD_PROFILE_FUNCTION();

	m_Texture = Ludo::Texture2D::Create("assets/textures/CheckerBoard.png");
}

void Sandbox2D::OnDetach()
{
	LD_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Ludo::TimeStep timeStep)
{
	LD_PROFILE_FUNCTION();

	// ========== Update =========
	m_CameraController.OnUpdate(timeStep);
	// ===========================

	// ========== Render =========
	Ludo::Renderer2D::BeginScene(m_CameraController.GetCamera());

	//Ludo::Renderer2D::DrawQuad(m_Position, { 10.0f, 10.0f }, m_Rotation, m_Texture, m_Color, 10.0f);
	Ludo::Renderer2D::DrawQuad({ -1.0f, 0.0f }, m_Size, 45, {9.0f, 0.5f, 0.8f, 1.0f});
	Ludo::Renderer2D::DrawQuad({  1.0f, 0.0f }, m_Size, 45, {2.0f, 0.9f, 0.7f, 1.0f});

	Ludo::Renderer2D::EndScene();
	// ===========================
}

void Sandbox2D::OnEvent(Ludo::Event& event)
{
	m_CameraController.OnEvent(event);
}

void Sandbox2D::OnImGuiRender()
{
	LD_PROFILE_FUNCTION();

	ImGui::Begin("Square");

	ImGui::DragFloat2("Position", (float*)&m_Position, 0.1f, 1.0f);
	ImGui::DragFloat2("Size", (float*)&m_Size, 0.1f);
	ImGui::DragFloat("Rotation", &m_Rotation);
	ImGui::ColorEdit4("Color", (float*)&m_Color);

	for (auto& result : m_ProfileResults)
	{
		char label[50];
		strcpy(label, "%.3fms ");
		strcat(label, result.Name);

		ImGui::Text(label, result.Time);
	}
	m_ProfileResults.clear();

	ImGui::End();
}
