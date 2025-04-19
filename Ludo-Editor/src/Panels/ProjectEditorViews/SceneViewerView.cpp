#include "SceneViewerView.h"

#include <imgui_internal.h>

#include <ImGuizmo.h>

namespace Ludo {

	SceneViewerView::SceneViewerView(const Ref<OpenAndSelectedsManager>& oas)
		: m_OpenAndSelecteds(oas)
	{
		LD_PROFILE_FUNCTION();

		m_ViewportSize.x = (float)Application::Get().GetWindow().GetWidth();
		m_ViewportSize.y = (float)Application::Get().GetWindow().GetHeight();

		FrameBufferSpecification fbSpec;
		fbSpec.Width = (uint32_t)m_ViewportSize.x;
		fbSpec.Height = (uint32_t)m_ViewportSize.y;
		DirectX::XMFLOAT4 clearColorID = { -1.0f, 0.0f, 0.0f, 0.0f };
		fbSpec.Attachments = {
				FrameBufferTextureFormat::RGBA8,
			{	FrameBufferTextureFormat::RED_INTEGER, true, clearColorID },
				FrameBufferTextureFormat::Depth
		};
		m_FrameBuffer = FrameBuffer::Create(fbSpec);
	}

	SceneViewerView::~SceneViewerView()
	{
		LD_PROFILE_FUNCTION();
	}

	void SceneViewerView::OnUpdate(TimeStep ts)
	{
		LD_PROFILE_FUNCTION();

		if (!m_OpenAndSelecteds->GetFocusedScene())
		{
			m_FrameBuffer->Bind();
			m_FrameBuffer->Unbind();

			return;
		}

		Ref<Scene> context = AssetManager::GetAsset<Scene>(m_OpenAndSelecteds->GetFocusedScene());

		if (m_ResizeFrameBuffer)
		{
			m_ResizeFrameBuffer = false;

			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);	
			context->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}

		if (m_ViewportActive)
		{
			m_EditorCamera.OnUpdate(ts);
		}

		Renderer2D::ResetStats();

		m_FrameBuffer->Bind();

		context->OnUpdateEditor(ts, m_EditorCamera);

		m_FrameBuffer->ClearDepthAttachment(1.0f);

		OnOverlayRender();

		m_FrameBuffer->Unbind();

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_MinViewportBounds.x;
		my -= m_MinViewportBounds.y;
		MouseX = (int)mx;
		MouseY = (int)my;
	}

	void SceneViewerView::OnOverlayRender()
	{
		Ref<Scene> context = AssetManager::GetAsset<Scene>(m_OpenAndSelecteds->GetFocusedScene());

		Renderer2D::BeginScene(m_EditorCamera);

		if (m_ShowPhysicsColliders)
		{
			// Box Colliders
			{
				auto view = context->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
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
				auto view = context->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
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

	void SceneViewerView::OnImGuiRender()
	{
		ImGuiWindowClass windowClass;
		windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoUndocking;
		ImGui::SetNextWindowClass(&windowClass);

		ImGui::Begin("Viewport", 0);

		ImGui::BeginTabBar("Scenes");

		ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 5.0f);

		AssetHandle remove;
		for (auto& sceneHandle : m_OpenAndSelecteds->GetSceneList())
		{
			Ref<Scene> scene = AssetManager::GetAsset<Scene>(sceneHandle);

			bool keepopen = true;

			bool IsFocusedScene = sceneHandle == m_OpenAndSelecteds->GetFocusedScene();
			bool IsSelecetedScene = false;

			if (ImGui::BeginTabItem(scene->GetName().empty() ? "Unnamed" : scene->GetName().c_str(), 
				&keepopen))
			{
				IsSelecetedScene = true;
				m_OpenAndSelecteds->SetFocusedScene(sceneHandle);

				ImGui::EndTabItem();
			}

			if (!keepopen)
			{
				remove = sceneHandle;
			}
		}

		if (remove)
		{
			m_OpenAndSelecteds->RemoveScene(remove);
		}

		ImGui::PopStyleVar();
		ImGui::EndTabBar();

		auto viewportOffset = ImGui::GetCursorPos();

		m_ViewportActive = ImGui::IsWindowHovered();

		Application::Get().ImGuiBlockEvent(ImGui::IsAnyItemFocused());

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if (viewportPanelSize.x != m_ViewportSize.x || viewportPanelSize.y != m_ViewportSize.y)
		{
			m_ResizeFrameBuffer = true;
		}
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		ImGui::Image(m_FrameBuffer->GetImTextureID(), viewportPanelSize);

		auto windowSize = ImGui::GetWindowSize();
		ImVec2 minBounds = ImGui::GetWindowPos();
		minBounds.x += viewportOffset.x;
		minBounds.y += viewportOffset.y;

		m_MinViewportBounds = { minBounds.x, minBounds.y };

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		ImGuizmo::SetRect(minBounds.x, minBounds.y, viewportPanelSize.x, viewportPanelSize.y);

		Entity selectedEntity = m_OpenAndSelecteds->GetFocusedEntity();

		if (selectedEntity && m_OpenAndSelecteds->GetFocusedScene())
		{
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			DirectX::XMFLOAT4X4 transform = tc.GetTransform();

			bool snap = Input::IsKeyPressed(KeyCode::Left_CTRL);
			float snapValue = m_GizmoType == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			const DirectX::XMFLOAT4X4& matrix = m_SceneState == SceneState::Edit ?
				m_EditorCamera.GetViewMatrix() : AssetManager::GetAsset<Scene>(m_OpenAndSelecteds->GetFocusedScene())->GetMainCamera().GetComponent<CameraComponent>().Camera.GetProjection();

			ImGuizmo::Manipulate(*matrix.m, *m_EditorCamera.GetProjection().m,
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::MODE::LOCAL,
				*transform.m, nullptr, snap ? snapValues : nullptr);

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

		ImGui::SetNextItemAllowOverlap();
		ImGui::SetCursorPos(viewportOffset);

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
	}

	bool SceneViewerView::OnEvent(Event& event)
	{
		if (m_ViewportActive)
		{
			m_EditorCamera.OnEvent(event);
		}

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(LUDO_BIND_EVENT_FN(SceneViewerView::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(LUDO_BIND_EVENT_FN(SceneViewerView::OnMouseButtonReleased));

		return false;
	}

	bool SceneViewerView::OnKeyPressed(KeyPressedEvent& event)
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

	bool SceneViewerView::OnMouseButtonReleased(MouseButtonReleasedEvent& event)
	{
		if (m_GizmoHovered || ImGui::IsAnyItemHovered() || Input::IsKeyPressed(KeyCode::Left_ALT) || event.GetMouseButton() != MouseButtonCode::Left)
		{
			return false;
		}

		LD_CORE_TRACE("What: {0}, {1}", MouseX, MouseY);

		if (MouseX >= 0 && MouseY >= 0 && MouseX < (int)m_FrameBuffer->GetSpecification().Width && MouseY < (int)m_FrameBuffer->GetSpecification().Height)
		{
			int ID = m_FrameBuffer->ReadPixel(1, MouseX, MouseY);

			if (ID == -1)
			{
				m_OpenAndSelecteds->SetFocusedEntity(Entity());
			}
			else
			{
				m_OpenAndSelecteds->SetFocusedEntity(Entity((entt::entity)ID, AssetManager::GetAsset<Scene>(m_OpenAndSelecteds->GetFocusedScene()).get()));
			}

			return true;
		}

		return false;
	}

}