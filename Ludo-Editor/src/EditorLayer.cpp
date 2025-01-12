#include "EditorLayer.h"	

namespace Ludo {

	EditorLayer::EditorLayer()
	{
		LD_PROFILE_FUNCTION();

		m_ViewportSize.x = Application::Get().GetWindow().GetWidth();
		m_ViewportSize.y = Application::Get().GetWindow().GetHeight();

		FrameBufferSpecification fbSpec;
		fbSpec.Width = (float)m_ViewportSize.x;
		fbSpec.Height = (float)m_ViewportSize.y;

		m_FrameBuffer = FrameBuffer::Create(fbSpec);

		m_Texture = Texture2D::Create("assets/textures/Checkerboard.png");

		m_ActiveScene = CreateRef<Scene>();

		m_SquareEntity =  m_ActiveScene->CreateEntity("Green Square");
		m_SquareEntity.AddComponent<SpriteRendererComponent>(DirectX::XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f });

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
		m_CameraEntity.AddComponent<CameraComponent>();
	}

	void EditorLayer::OnUpdate(TimeStep ts)
	{
		LD_PROFILE_FUNCTION();

		if (m_ResizeFrameBuffer)
		{
			m_ResizeFrameBuffer = false;

			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		Renderer2D::ResetStats();

		m_FrameBuffer->Bind();

		m_ActiveScene->OnUpdate(ts);

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
		ImGui::Text("Renderer 2D Stats:");
		ImGui::Text(" Draw Calls: %d", stats.DrawCalls);
		ImGui::Text(" Quads     : %d", stats.QuadCount);
		ImGui::Text(" Vertices  : %d", stats.GetTotalVertexCount());
		ImGui::Text(" Indices   : %d", stats.GetTotalIndexCount());

		ImGui::Separator();

		ImGui::Text("%s", m_SquareEntity.GetComponent<TagComponent>().Tag.c_str());
		auto& color = m_SquareEntity.GetComponent<SpriteRendererComponent>().Color;
		ImGui::ColorEdit4("Square Color", (float*)&color);

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
		ImGui::Begin("Viewport");

		m_ViewportFocused = ImGui::IsWindowFocused();
		Application::Get().ImGuiBlockEvent(!ImGui::IsWindowHovered());

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ResizeFrameBuffer = viewportPanelSize.x != m_ViewportSize.x || viewportPanelSize.y != m_ViewportSize.y;
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		ImGui::Image(m_FrameBuffer->GetImTextureID(), viewportPanelSize);

		ImGui::End();
		ImGui::PopStyleVar();
	}

}