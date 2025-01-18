#include "EditorLayer.h"

#include "Style/ImGuiThemeManager.h"

#include <ImGuizmo/ImGuizmo.h>

#define _USE_MATH_DEFINES
#include <cmath>

namespace Ludo {

	void EditorLayer::OnAttach()
	{
		LD_PROFILE_FUNCTION();

		ImGuiFontManager::Get()->Init();
		SetImGuiDarkTheme();

		m_ViewportSize.x = (float)Application::Get().GetWindow().GetWidth();
		m_ViewportSize.y = (float)Application::Get().GetWindow().GetHeight();

		FrameBufferSpecification fbSpec;
		fbSpec.Width = (uint32_t)m_ViewportSize.x;
		fbSpec.Height = (uint32_t)m_ViewportSize.y;

		m_FrameBuffer = FrameBuffer::Create(fbSpec);

		m_ActiveScene = CreateRef<Scene>();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDetach()
	{
		LD_PROFILE_FUNCTION();
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
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(LUDO_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}

	void EditorLayer::OnImGuiRender()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		float ogMinWinWidth = style.WindowMinSize.x;
		style.WindowMinSize.x = 340.0f;
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
		style.WindowMinSize.x = ogMinWinWidth;

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
				{
					NewScene();
				}

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
				{
					OpenScene();
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
				{
					SaveSceneAs();
				}

				if (ImGui::MenuItem("Exit")) { Application::Get().Close(); }

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		m_SceneHierarchyPanel.OnImGuiRender();

		ImGui::Begin("Stats");

		auto stats = Ludo::Renderer2D::GetStats();
		ImGui::Text("Renderer 2D Stats:");
		ImGui::Text(" Draw Calls: %d", stats.DrawCalls);
		ImGui::Text(" Quads     : %d", stats.QuadCount);
		ImGui::Text(" Vertices  : %d", stats.GetTotalVertexCount());
		ImGui::Text(" Indices   : %d", stats.GetTotalIndexCount());

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
		ImGui::Begin("Viewport");

		m_ViewportFocused = ImGui::IsWindowFocused();

		if (ImGui::IsAnyItemActive())
		{
			Application::Get().ImGuiBlockEvent(!m_ViewportFocused || !ImGui::IsWindowHovered());
		}
		else
		{
			Application::Get().ImGuiBlockEvent(!m_ViewportFocused && !ImGui::IsWindowHovered());
		}
		

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ResizeFrameBuffer = viewportPanelSize.x != m_ViewportSize.x || viewportPanelSize.y != m_ViewportSize.y;
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		ImGui::Image(m_FrameBuffer->GetImTextureID(), viewportPanelSize);

		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
		if (selectedEntity && cameraEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

			const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;

			const DirectX::XMFLOAT4X4& cameraProjection = camera.GetProjection();
			DirectX::XMMATRIX cameraTransform; cameraEntity.GetComponent<TransformComponent>().GetTransform(&cameraTransform);
			DirectX::XMFLOAT4X4 cameraView;
			DirectX::XMStoreFloat4x4(&cameraView, DirectX::XMMatrixInverse(nullptr, cameraTransform));

			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			DirectX::XMFLOAT4X4 transform = tc.GetTransform();

			bool snap = Input::IsKeyPressed(KeyCode::Left_CTRL);
			float snapValue = m_GizmoType == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate((float*)&cameraView, (float*)&cameraProjection, 
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::MODE::LOCAL,
				(float*)&transform, nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				DirectX::XMVECTOR translation;
				DirectX::XMVECTOR quaternionRotation;
				DirectX::XMVECTOR scale;
				DirectX::XMMatrixDecompose(&scale, &quaternionRotation, &translation, DirectX::XMLoadFloat4x4(&transform));

				DirectX::XMStoreFloat3(&tc.Translation, translation);

				DirectX::XMFLOAT4 q;
				DirectX::XMStoreFloat4(&q, quaternionRotation);
				float roll = atan2(2 * (q.w * q.x + q.y * q.z), 1 - 2 * (q.x * q.x + q.y * q.y));
				float pitch = 2.0f * atan2(std::sqrt(1 + 2 * (q.w * q.y - q.x * q.z)), std::sqrt(1 - 2 * (q.w * q.y - q.x * q.z))) - DirectX::XM_PI / 2.0f;
				float yaw = atan2(2 * (q.w * q.z + q.x * q.y), 1 - 2 * (q.y * q.y + q.z * q.z));
				tc.Rotation = DirectX::XMFLOAT3{ roll, pitch, yaw };

				DirectX::XMStoreFloat3(&tc.Scale, scale);
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& event)
	{
		if (event.GetRepeatCount() > 1)
		{
			return false;
		}

		bool control = Input::IsKeyPressed(KeyCode::Left_CTRL) || Input::IsKeyPressed(KeyCode::Right_CTRL);
		bool shift = Input::IsKeyPressed(KeyCode::Left_Shift) || Input::IsKeyPressed(KeyCode::Right_Shift);

		switch (event.GetKeyCode())
		{
		case KeyCode::N:
			if (shift)
			{
				NewScene();
			}

			break;

		case KeyCode::O:

			if (control)
			{
				OpenScene();
			}

			break;

		case KeyCode::S:
			if (Input::IsKeyPressed(KeyCode::CTRL) && control && shift)
			{
				SaveSceneAs();
			}
			break;

			// Gizmo Shortcuts
		case KeyCode::Q:
			m_GizmoType = -1;
			break;

		case KeyCode::W:
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case KeyCode::E:	
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case KeyCode::R:
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}

	void EditorLayer::OpenScene()
	{
		auto path = FileDialogs::OpenFile("LudoNarrative Scene (*.LudoNarrative)\0*.LudoNarrative\0");
		if (!path.empty())
		{
			m_ActiveScene = CreateRef<Scene>();
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

			SceneSerializer serializer(m_ActiveScene);
			serializer.DeserializeFromYamlFile(path);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		auto path = FileDialogs::SaveFile("LudoNarrative Scene (*.LudoNarrative)\0*.LudoNarrative\0");
		if (!path.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.SerializeToYamlFile(path);
		}
	}

	void EditorLayer::SetImGuiDarkTheme()
	{
		ImGuiTheme theme;

		// Windows
		theme.Colors.WindowBackground = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		theme.Colors.Header = ImVec4{ 0.2f, 0.205f, 0.21, 1.0f };
		theme.Colors.HeaderHovered = ImVec4{ 0.3f, 0.305f, 0.32f, 1.0f };
		theme.Colors.HeaderActive = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		theme.Colors.Button = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		theme.Colors.ButtonHovered = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		theme.Colors.ButtonActive = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame Background
		theme.Colors.FrameBackground = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		theme.Colors.FrameBackgroundHovered = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		theme.Colors.FrameBackgroundActive = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		theme.Colors.Tab = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		theme.Colors.TabHovered = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		theme.Colors.TabActive = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		theme.Colors.TabUnfocused = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f }; 
		theme.Colors.TabUnfocusedActive = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		theme.Colors.TitleBackground = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		theme.Colors.TitleBackgroundActive = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		theme.Colors.TitleBackgroundCollapsed = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		CustomFontDescription fontDesc;
		fontDesc.SizeInPixels = 15.0f;
		fontDesc.RegularFont = "assets/fonts/roboto/roboto-Regular.ttf";
		fontDesc.BoldFont = "assets/fonts/roboto/Roboto-Bold.ttf";

		theme.Font = ImGuiFontManager::Get()->AddFont("Roboto", fontDesc);
		
		ImGuiThemeManager::AddTheme("Dark Theme", theme);
		ImGuiThemeManager::SetCurrentTheme("Dark Theme");
	}

}