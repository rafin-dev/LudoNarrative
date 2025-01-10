#include "EditorLayer.h"	

namespace Ludo {

	EditorLayer::EditorLayer()
		: m_CameraController(1280.0f / 720.0f, true)
	{
		LD_PROFILE_FUNCTION();

		m_ViewportSize.x = Application::Get().GetWindow().GetWidth();
		m_ViewportSize.y = Application::Get().GetWindow().GetHeight();

		FrameBufferSpecification fbSpec;
		fbSpec.Width = (float)m_ViewportSize.x;
		fbSpec.Height = (float)m_ViewportSize.y;

		m_FrameBuffer = FrameBuffer::Create(fbSpec);

		m_Texture = Texture2D::Create("assets/textures/Checkerboard.png");

		m_CameraController.SetZoomLevel(10.0f);
	}

	void EditorLayer::OnUpdate(TimeStep ts)
	{
		LD_PROFILE_FUNCTION();

		m_CameraController.OnUpdate(ts);

		if (m_ResizeFrameBuffer)
		{
			m_ResizeFrameBuffer = false;

			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);
		}

		Renderer2D::ResetStats();

		m_FrameBuffer->Bind();

		Renderer2D::BeginScene(m_CameraController.GetCamera());

		Ludo::Renderer2D::DrawQuad({ 0.0f, 0.0f }, {10.0f, 10.0f}, 0.0f, m_Texture, {1.0f, 1.0f, 1.0f, 1.0f}, 10.0f);

		Renderer2D::EndScene();

		m_FrameBuffer->Unbind();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		m_CameraController.OnEvent(event);
	}

	void EditorLayer::OnImGuiRender()
	{
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

		if (ImGui::BeginMainMenuBar())
		{

			if (ImGui::BeginMenu("File"))
			{

				if (ImGui::MenuItem("Exit")) { Application::Get().Close(); }

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		ImGui::Begin("Info");

		auto stats = Ludo::Renderer2D::GetStats();
		ImGui::Text("==================");
		ImGui::Text("Renderer 2D Stats:");
		ImGui::Text(" Draw Calls: %d", stats.DrawCalls);
		ImGui::Text(" Quads     : %d", stats.QuadCount);
		ImGui::Text(" Vertices  : %d", stats.GetTotalVertexCount());
		ImGui::Text(" Indices   : %d", stats.GetTotalIndexCount());
		ImGui::Text("==================");

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
		ImGui::Begin("Viewport");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ResizeFrameBuffer = viewportPanelSize.x != m_ViewportSize.x || viewportPanelSize.y != m_ViewportSize.y;
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		ImGui::Image(m_FrameBuffer->GetImTextureID(), viewportPanelSize);
		
		ImGui::End();
		ImGui::PopStyleVar();
	}

}