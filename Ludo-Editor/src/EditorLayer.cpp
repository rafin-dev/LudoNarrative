#include "EditorLayer.h"

#include "Style/ImGuiThemeManager.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <ImGuizmo.h>

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
		DirectX::XMFLOAT4 clearColorID = { -1.0f, 0.0f, 0.0f, 0.0f };
		fbSpec.Attachments = {
				FrameBufferTextureFormat::RGBA8,
			{	FrameBufferTextureFormat::RED_INTEGER, true, clearColorID},
				FrameBufferTextureFormat::Depth
		};

		m_FrameBuffer = FrameBuffer::Create(fbSpec);

		m_EditorCamera = EditorCamera(30.0f, 1.0f, 0.01f, 1000.0f);

		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		m_PlayButtonIcon = ImGuiTexture::Create(Texture2D::Create("assets/icons/PlayButton.png"));
		m_StopButtonIcon = ImGuiTexture::Create(Texture2D::Create("assets/icons/StopButton.png"));

		m_OpenScenes.push_back(CreateRef<Scene>());
		m_ScenePaths.push_back(std::filesystem::path());
	}

	void EditorLayer::OnDetach()
	{
		LD_PROFILE_FUNCTION();
	}

	static float frameTimes[20] = { 1.0f, 1.0f, 1.0f, 1.0f ,1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f ,1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	static int currentTime = 0;
	static float frameTimems = 0.0f;
	void EditorLayer::OnUpdate(TimeStep ts)
	{
		LD_PROFILE_FUNCTION();

		frameTimems = ts.GetMiliseconds();
		frameTimes[currentTime] = ts;
		currentTime++;
		currentTime %= 20;

		if (m_ResizeFrameBuffer)
		{
			m_ResizeFrameBuffer = false;

			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}

		if (m_ViewportActive)
		{
			m_EditorCamera.OnUpdate(ts);
		}

		Renderer2D::ResetStats();

		m_FrameBuffer->Bind();

		switch (m_SceneState)
		{
			case SceneState::Edit:
			{
				m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
				break;
			}
			case SceneState::Play:
			{
				m_ActiveScene->OnUpdateRuntime(ts);
				break;
			}
			case SceneState::Simulate:
			{
				m_ActiveScene->OnUpdateSimulate(ts, m_EditorCamera);
				break;
			}
		}

		m_FrameBuffer->ClearDepthAttachment(1.0f);
		OnOverlayRender();

		m_FrameBuffer->Unbind();

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_MinViewportBounds.x;
		my -= m_MinViewportBounds.y;
		MouseX = (int)mx;
		MouseY = (int)my;
	}

	void EditorLayer::OnOverlayRender()
	{
		Entity camera = m_ActiveScene->GetMainCamera();
		if (m_SceneState == SceneState::Play && camera)
		{
			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
		}
		else
		{
			Renderer2D::BeginScene(m_EditorCamera);
		}

		if (m_ShowPhysicsColliders)
		{
			// Box Colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
				for (auto entityID : view)
				{
					auto [transformComponent, boxCollider] = view.get<TransformComponent, BoxCollider2DComponent>(entityID);

					DirectX::XMMATRIX transform =
						DirectX::XMMatrixScaling(
							transformComponent.Scale.x * boxCollider.Size.x,
							transformComponent.Scale.y * boxCollider.Size.y, 1.0f) *
						DirectX::XMMatrixRotationZ(boxCollider.Rotation) *
						DirectX::XMMatrixRotationZ(transformComponent.Rotation.z) *
						DirectX::XMMatrixTranslation(
							transformComponent.Translation.x + boxCollider.Offset.x,
							transformComponent.Translation.y + boxCollider.Offset.y,
							0.0f);

					Renderer2D::DrawRect(transform, { 0.0f, 1.0f, 0.0f, 1.0f });
				}
			}

			// Circle Colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
				for (auto entityID : view)
				{
					auto [transformComponent, circleCollider] = view.get<TransformComponent, CircleCollider2DComponent>(entityID);

					DirectX::XMFLOAT3 radiusvec = { circleCollider.Radius, circleCollider.Radius, circleCollider.Radius };

					DirectX::XMFLOAT3 scx = { transformComponent.Scale.x, transformComponent.Scale.x, transformComponent.Scale.x };
					DirectX::XMVECTOR scale = DirectX::XMVectorMultiply(DirectX::XMLoadFloat3(&scx),
						DirectX::XMVectorScale(DirectX::XMLoadFloat3(&radiusvec), 2.0f));

					DirectX::XMMATRIX transform =
						DirectX::XMMatrixScalingFromVector(scale) *
						DirectX::XMMatrixRotationZ(transformComponent.Rotation.z) *
						DirectX::XMMatrixTranslation(
							transformComponent.Translation.x + circleCollider.Offset.x,
							transformComponent.Translation.y + circleCollider.Offset.y,
							0.0f);

					Renderer2D::DrawCircle(transform, { 0.0f, 1.0f, 0.0f, 1.0f }, 0.05f, 0.0f);

				}
			}
		}

		Renderer2D::EndScene();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		if (m_ViewportActive)
		{
			m_EditorCamera.OnEvent(event);
		}

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(LUDO_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(LUDO_BIND_EVENT_FN(EditorLayer::OnMouseButtonReleasedEvent));
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
		m_ContentBrowserPanel.OnImGuiRender();

		ImGui::Begin("Settings");

		float avgft = 0.0f;
		for (float ft : frameTimes)
		{
			avgft += ft;
		}
		avgft /= 20.0f;
		ImGui::Text("Frame Time: %.2fms", frameTimems);
		ImGui::Text("FPS: %.2f", (1.0f / avgft));

		static bool vsync = true;
		if (ImGui::Checkbox("Vsync", &vsync))
		{
			Application::Get().GetWindow().SetVsync(vsync);
		}

		ImGui::Checkbox("Show Physics Colliders", &m_ShowPhysicsColliders);

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
		ImGui::Begin("Viewport", 0, ImGuiWindowFlags_NoTitleBar);

		ImGui::BeginTabBar("Scenes");

		ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 5.0f);

		int removeID = -1;
		int counter = 0;
		for (auto& scene : m_OpenScenes)
		{
			bool keepOpen = true;
			bool isNewScene = m_NewSceneName == scene->GetName();

			if (ImGui::BeginTabItem(scene->GetName().empty() ? "Unnamed" : scene->GetName().c_str(), &keepOpen,
				isNewScene ? ImGuiTabItemFlags_SetSelected : 0))
			{
				if (m_EditorScene->GetName() != scene->GetName() && m_SceneState == SceneState::Edit)
				{
					m_EditorScene = scene;
					m_ActiveScene = m_EditorScene;
					m_SceneHierarchyPanel.SetContext(m_ActiveScene);
				}

				ImGui::EndTabItem();
			}

			if (isNewScene)
			{
				m_NewSceneName = std::string();
			}

			if (!keepOpen)
			{
				removeID = counter;
			}

			counter++;
		}
		if (removeID != -1)
		{
			m_OpenScenes.erase(m_OpenScenes.begin() + removeID);

			if (m_OpenScenes.size() == 0)
			{
				m_OpenScenes.push_back(CreateRef<Scene>());
			}
		}

		ImGui::PopStyleVar();

		ImGui::EndTabBar();

		auto viewportOffset = ImGui::GetCursorPos();
		ImVec2 minPlusPadding = ImGui::GetCursorPos();

		m_ViewportActive = ImGui::IsWindowHovered();

		Application::Get().ImGuiBlockEvent(ImGui::IsAnyItemFocused());
		
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ResizeFrameBuffer = viewportPanelSize.x != m_ViewportSize.x || viewportPanelSize.y != m_ViewportSize.y;
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		ImGui::Image(m_FrameBuffer->GetImTextureID(), viewportPanelSize);

		auto windowSize = ImGui::GetWindowSize();
		ImVec2 minBounds = ImGui::GetWindowPos();
		minBounds.x += viewportOffset.x;
		minBounds.y += viewportOffset.y;

		m_MinViewportBounds = { minBounds.x, minBounds.y };

		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_SceneState == SceneState::Edit && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			DirectX::XMFLOAT4X4 transform = tc.GetTransform();

			bool snap = Input::IsKeyPressed(KeyCode::Left_CTRL);
			float snapValue = m_GizmoType == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			const DirectX::XMFLOAT4X4& matrix = m_SceneState == SceneState::Edit ? 
				m_EditorCamera.GetViewMatrix() : m_ActiveScene->GetMainCamera().GetComponent<CameraComponent>().Camera.GetProjection();

			ImGuizmo::Manipulate((float*)&matrix, (float*)&m_EditorCamera.GetProjection(),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::MODE::LOCAL,
				(float*)&transform, nullptr, snap ? snapValues : nullptr);

			m_GizmoHovered = ImGuizmo::IsOver();
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

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				std::filesystem::path path((const wchar_t*)payload->Data);

				if (path.extension() == ".LudoNarrative")
				{
					OpenScene(path);
				}
			}
			ImGui::EndDragDropTarget();
		}

		minPlusPadding.x += 5.0f;
		minPlusPadding.y += 5.0f;

		ImGui::SetNextItemAllowOverlap();
		ImGui::SetCursorPos(minPlusPadding);

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Gizmo");
		ImGui::SameLine();
		if (ImGui::Button("Position"))
		{
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
		}
		ImGui::SameLine();
		if (ImGui::Button("Rotation"))
		{
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
		}
		ImGui::SameLine();
		if (ImGui::Button("Scale"))
		{
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
		}

		ImGui::End();
		ImGui::PopStyleVar();

		RenderToolBar();
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& event)
	{
		if (event.GetRepeatCount() > 1)
		{
			return false;
		}

		bool control = Input::IsKeyPressed(KeyCode::Left_CTRL) || Input::IsKeyPressed(KeyCode::Right_CTRL);
		bool shift = Input::IsKeyPressed(KeyCode::Left_Shift) || Input::IsKeyPressed(KeyCode::Right_Shift);

		if (event.GetKeyCode() == KeyCode::C)
		{
			LD_CORE_TRACE("Searching in viewport");
			for (uint32_t y = 0; y < m_ViewportSize.y; y++)
			{
				for (uint32_t x = 0; x < m_ViewportSize.x; x++)
				{
					if (m_FrameBuffer->ReadPixel(1, x, y) != -1)
					{
						LD_CORE_INFO("Found one");
					}
				}
			}
		}

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

			if (control)
			{
				if (shift)
				{
					SaveSceneAs();
				}
				else
				{
					SaveScene();
				}
			}
			break;

		case KeyCode::D:
			if (control)
			{
				OnDuplicateEntity();
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

		return false;
	}

	bool EditorLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& event)
	{
		if (m_GizmoHovered || ImGui::IsAnyItemHovered() || Input::IsKeyPressed(KeyCode::Left_ALT) || event.GetMouseButton() != MouseButtonCode::Left)
		{
			return false;
		}

		if (MouseX >= 0 && MouseY >= 0 && MouseX < (int)m_FrameBuffer->GetSpecification().Width && MouseY < (int)m_FrameBuffer->GetSpecification().Height)
		{
			int ID =  m_FrameBuffer->ReadPixel(1, MouseX, MouseY);

			if (ID == -1)
			{
				m_SceneHierarchyPanel.SetSelectedEntity(Entity());
			}
			else 
			{
				m_SceneHierarchyPanel.SetSelectedEntity(Entity((entt::entity)ID, m_ActiveScene.get()));
			}

			return true;
		}

		return false;
	}

	void EditorLayer::NewScene()
	{
		m_EditorScenePath = std::filesystem::path();
		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}

	void EditorLayer::OpenScene()
	{
		auto path = FileDialogs::OpenFile("LudoNarrative Scene (*.LudoNarrative)\0*.LudoNarrative\0");
		if (!path.empty())
		{
			OpenScene(path);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
		{
			OnSceneStop();
		}

		LD_CORE_ASSERT(std::filesystem::exists(path), "Scene File Does not exist");

		for (auto& scene : m_OpenScenes)
		{
			if (scene->GetName() == path.stem())
			{
				m_NewSceneName = path.stem().string();

				return;
			}
		}

		Ref<Scene> newScene = CreateRef<Scene>(path.stem().string());
		SceneSerializer serializer(newScene);
		if (serializer.DeserializeFromYamlFile(path))
		{
			m_EditorScenePath = path;
			m_EditorScene = newScene;
			m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
		}

		if (m_OpenScenes[0]->GetName().empty())
		{
			m_OpenScenes[0] = m_EditorScene;
			m_ScenePaths[0] = path;
		}
		else
		{
			m_OpenScenes.push_back(m_EditorScene);
			m_ScenePaths.push_back(path);
		}

		m_NewSceneName = m_EditorScene->GetName();
	}
 
	void EditorLayer::SaveScene()
	{
		std::filesystem::path path;
		for (auto p : m_ScenePaths)
		{
			if (m_EditorScene->GetName() == p.stem())
			{
				path = p;
			}
		}

		if (std::filesystem::exists(path))
		{
			SceneSerializer serializer(m_EditorScene);
			serializer.SerializeToYamlFile(path);
		}
		else
		{
			SaveSceneAs();
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		auto path = FileDialogs::SaveFile("LudoNarrative Scene (*.LudoNarrative)\0*.LudoNarrative\0");
		if (!path.empty())
		{
			SceneSerializer serializer(m_EditorScene);
			serializer.SerializeToYamlFile(path);

			m_EditorScenePath = path;
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

	void EditorLayer::OnScenePlay()
	{
		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;

		m_ActiveScene->OnRuntimeStop();
		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneSimulateStart()
	{
		m_SceneState = SceneState::Simulate;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulateStart();
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneSimulateStop()
	{
		m_SceneState = SceneState::Edit;

		m_ActiveScene->OnSimulateStop();
		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
		{
			return;
		}

		auto entity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (entity)
		{
			m_ActiveScene->DuplicateEntity(entity);
		}
	}

	void EditorLayer::RenderToolBar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.305f, 0.31f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		// Play Button
		{
			float size = ImGui::GetWindowHeight() - 4.0f;
			const Ref<ImGuiTexture>& icon = m_SceneState == SceneState::Play ? m_StopButtonIcon : m_PlayButtonIcon;
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton("##playButton", icon->GetImTextureID(), ImVec2(size, size)))
			{
				switch (m_SceneState)
				{
				case Ludo::EditorLayer::SceneState::Edit:
					OnScenePlay();
					break;

				case Ludo::EditorLayer::SceneState::Play:
					OnSceneStop();
					break;
				}
			}
		}

		ImGui::SameLine();

		// Simulate Button
		{
			float size = ImGui::GetWindowHeight() - 4.0f;
			const Ref<ImGuiTexture>& icon = m_SceneState == SceneState::Simulate ? m_StopButtonIcon : m_PlayButtonIcon;
			if (ImGui::ImageButton("##simulateButton", icon->GetImTextureID(), ImVec2(size, size)))
			{
				switch (m_SceneState)
				{
				case Ludo::EditorLayer::SceneState::Edit:
					OnSceneSimulateStart();
					break;

				case Ludo::EditorLayer::SceneState::Simulate:
					OnSceneSimulateStop();
					break;
				}
			}
		}

		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor(3);

		ImGui::End();
	}

}