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

	Ludo::RenderCommand::SetClearColor({ 0.5f, 0.2f, 0.9f, 1.0f });
}

void Sandbox2D::OnDetach()
{
	LD_PROFILE_FUNCTION();
}

static bool Render5quads = false;

void Sandbox2D::OnUpdate(Ludo::TimeStep timeStep)
{
	LD_PROFILE_FUNCTION();

	m_LastDeltaTime = timeStep;

	// ========== Update =========
	m_CameraController.OnUpdate(timeStep);
	// ===========================

	// ========== Render =========
	Ludo::Renderer2D::ResetStats();

	{
		LD_PROFILE_SCOPE("Rendering submissuion");

		Ludo::Renderer2D::BeginScene(m_CameraController.GetCamera());

		static float rotation = 0.0f;
		rotation += 20.0f * timeStep;

		Ludo::Renderer2D::DrawQuad(m_Position, { 10.0f, 10.0f }, m_Rotation, m_Texture, m_Color, 10.0f);
		Ludo::Renderer2D::DrawQuad({ -1.0f, 0.0f }, m_Size, rotation, m_Texture, m_Color, 3.0f);
		Ludo::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.0f }, { 0.75f, 1.0f }, 0, { 0.9f, 0.5f, 0.8f, 1.0f });
		Ludo::Renderer2D::DrawQuad({ 0.0f, 2.0f, 0.0f }, m_Size, 0, { 0.0f, 0.0f, 1.0f, 1.0f });

		if (Render5quads)
		{
			Ludo::Renderer2D::DrawQuad({ 5.0f, 2.0f, 0.0f }, m_Size, 0, { 0.0f, 0.0f, 1.0f, 1.0f });
		}

		Ludo::Renderer2D::EndScene();
	}
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
	
	int frameRate = 1 / m_LastDeltaTime;
	ImGui::Text("Frame rate: %i", frameRate);
	
	auto stats = Ludo::Renderer2D::GetStats();
	ImGui::Text("==================");
	ImGui::Text("Renderer 2D Stats:");
	ImGui::Text(" Draw Calls: %d", stats.DrawCalls);
	ImGui::Text(" Quads     : %d", stats.QuadCount);
	ImGui::Text(" Vertices  : %d", stats.GetTotalVertexCount());
	ImGui::Text(" Indices   : %d", stats.GetTotalIndexCount());
	ImGui::Text("==================");

	if (ImGui::Button(Render5quads ? "Render 4 Quads" : "Render 5 Quads"))
	{
		Render5quads = !Render5quads;
	}

	ImGui::DragFloat2("Position", (float*)&m_Position, 0.1f, 1.0f);
	ImGui::DragFloat2("Size", (float*)&m_Size, 0.1f);
	ImGui::DragFloat("Rotation", &m_Rotation);
	ImGui::ColorEdit4("Color", (float*)&m_Color);
	
	ImGui::End();
}
