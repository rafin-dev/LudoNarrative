#include "ScenePanelHierarchy.h"

#include "ImGuiItems/ImGuiItems.h"

namespace Ludo {

	void ScenePanelHierarchy::OnRenderTransformComponent()
	{
		auto& transform = m_SelectedEntity.GetComponent<TransformComponent>();

		ImGuiItems::DragFloat3("Position", transform.Translation);

		DirectX::XMFLOAT3 rotation;
		rotation.x = DirectX::XMConvertToDegrees(transform.Rotation.x);
		rotation.y = DirectX::XMConvertToDegrees(transform.Rotation.y);
		rotation.z = DirectX::XMConvertToDegrees(transform.Rotation.z);
		ImGuiItems::DragFloat3("Rotation", rotation);
		transform.Rotation.x = DirectX::XMConvertToRadians(rotation.x);
		transform.Rotation.y = DirectX::XMConvertToRadians(rotation.y);
		transform.Rotation.z = DirectX::XMConvertToRadians(rotation.z);

		ImGuiItems::DragFloat3("Scale", transform.Scale, 1.0f);

		ImGui::Spacing();
	}

	void ScenePanelHierarchy::OnRenderSpriteRendererComponent()
	{
		auto& spriteComponent = m_SelectedEntity.GetComponent<SpriteRendererComponent>();

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), spriteComponent.TexturePath.string().c_str());
		if (ImGui::InputText("Texture Path", buffer, 256))
		{
			SetSelectedEntityTexture(buffer);
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				SetSelectedEntityTexture(path);
			}
			ImGui::EndDragDropTarget();
		}

		auto width = ImGui::GetContentRegionAvail().x / 2;
		if (m_SelectedEntityImGuiTexture)
		{
			bool removeTexture = false;
			if (ImGui::Button("Remove Texture"))
			{
				removeTexture = true;
			}

			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - width) * 0.5f);
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - width) * 0.5f);
			auto cursorPos = ImGui::GetCursorPos();
			ImGui::ColorButton("##IBG", ImVec4(0.1f, 0.1f, 0.1f, 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2{ width, width });

			ImGui::SetNextItemAllowOverlap();
			ImGui::SetCursorPos(cursorPos);
			ImGui::Image(m_SelectedEntityImGuiTexture->GetImTextureID(), { width, width }, ImVec2(0, 1), ImVec2(1, 0),
				ImVec4(spriteComponent.Color.x, spriteComponent.Color.y, spriteComponent.Color.z, spriteComponent.Color.w));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					SetSelectedEntityTexture(path);
				}
				ImGui::EndDragDropTarget();
			}

			if (removeTexture)
			{
				spriteComponent.Texture = nullptr;
				spriteComponent.TexturePath = "None";
				m_SelectedEntityImGuiTexture = nullptr;
			}
		}

		ImGui::ColorEdit4("Color", (float*)&spriteComponent.Color);
		ImGui::DragFloat("Tilign Factor", &spriteComponent.TilingFactor);
	}

	void ScenePanelHierarchy::OnRenderCircleRendererComponent()
	{
		auto& circleRenderer = m_SelectedEntity.GetComponent<CircleRendererComponent>();

		ImGui::ColorEdit4("Color", (float*)&circleRenderer.Color);
		ImGui::DragFloat("Thickness", &circleRenderer.Thickness, 0.025, 0.0f, 1.0f);
		ImGui::DragFloat("Fade", &circleRenderer.Fade, 0.00025, 0.0f, 1.0f);
	}

	void ScenePanelHierarchy::OnRenderScriptComponent()
	{
		
	}

	void ScenePanelHierarchy::OnRenderCameraComponent()
	{
		auto& cameraComponent = m_SelectedEntity.GetComponent<CameraComponent>();
		auto& camera = cameraComponent.Camera;

		bool isMainCamera = m_SelectedEntity.IsMainCamera();
		if (ImGui::Checkbox("Primary", &isMainCamera))
		{
			if (isMainCamera)
			{
				m_SelectedEntity.SetAsMainCamera();
			}
			else
			{
				m_Context->SetMainCamera(Entity());
			}
		}

		const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
		const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

		if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
		{
			for (int i = 0; i < 2; i++)
			{
				bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
				if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
				{
					currentProjectionTypeString = projectionTypeStrings[i];
					camera.SetProjectionType((SceneCamera::ProjectionType)i);
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		SceneCamera::ProjectionType pjType = camera.GetProjectionType();
		if (pjType == SceneCamera::ProjectionType::Perspective)
		{
			float verticalFOV = DirectX::XMConvertToDegrees(camera.GetPerspectiveVerticalFov());
			if (ImGui::DragFloat("Vertical FOV", &verticalFOV)) { camera.SetPerspectiveVerticalFov(DirectX::XMConvertToRadians(verticalFOV)); }

			float perspectiveNear = camera.GetPerspectiveNearClip();
			if (ImGui::DragFloat("Near", &perspectiveNear) && perspectiveNear > 0.0f) { camera.SetPerspectiveNearClip(perspectiveNear); }

			float perspectiveFar = camera.GetPerspectiveFarClip();
			if (ImGui::DragFloat("Far", &perspectiveFar)) { camera.SetPerspectiveFarClip(perspectiveFar); }
		}
		else if (pjType == SceneCamera::ProjectionType::Orthographic)
		{
			float orthoSize = camera.GetOrthographicSize();
			float orthoNear = camera.GetOrthographicNearClip();
			float orthoFar = camera.GetOrthographicFarClip();

			if (ImGui::DragFloat("Size", &orthoSize) && orthoSize != 0.0f) { camera.SetOrthographicSize(orthoSize); }

			if (ImGui::DragFloat("Near", &orthoNear) && orthoNear != orthoFar) { camera.SetOrthographicNearClip(orthoNear); }

			if (ImGui::DragFloat("Far", &orthoFar) && orthoFar != orthoNear && orthoFar != 0.0f) { camera.SetOrthographicFarClip(orthoFar); }
		}

		ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);
	}

	void ScenePanelHierarchy::OnRenderRigidbody2DComponent()
	{
		auto& rigidbody = m_SelectedEntity.GetComponent<Rigidbody2DComponent>();

		const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
		const char* currentBodyTypeString = bodyTypeStrings[(int)rigidbody.Type];

		if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
		{
			for (int i = 0; i < 3; i++)
			{
				bool isSelected = currentBodyTypeString == bodyTypeStrings[i];

				if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
				{
					currentBodyTypeString = bodyTypeStrings[i];
					rigidbody.Type = (Rigidbody2DComponent::BodyType)i;
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::Checkbox("Fixed Rotation", &rigidbody.FixedRotation);
	}

	void ScenePanelHierarchy::OnRenderBoxCollider2DComponent()
	{
		auto& boxCollider = m_SelectedEntity.GetComponent<BoxCollider2DComponent>();

		if (!m_SelectedEntity.HasComponent<Rigidbody2DComponent>())
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Box Collider2D Requires a Rigidbody2D to work!");
		}

		ImGuiItems::DragFloat2("Offset", boxCollider.Offset);
		ImGuiItems::DragFloat2("Size", boxCollider.Size, 1.0f);
		float rotation = DirectX::XMConvertToDegrees(boxCollider.Rotation);
		if (ImGui::DragFloat("Rotation", &rotation))
		{
			boxCollider.Rotation = DirectX::XMConvertToRadians(rotation);
		}

		ImGui::DragFloat("Density", &boxCollider.Density);
		ImGui::DragFloat("Friction", &boxCollider.Friction);
		ImGui::DragFloat("Restitution", &boxCollider.Restitution);
	}

	void ScenePanelHierarchy::OnRenderCircleCollider2DComponent()
	{
		CircleCollider2DComponent& circleCollider = m_SelectedEntity.GetComponent<CircleCollider2DComponent>();

		if (!m_SelectedEntity.HasComponent<Rigidbody2DComponent>())
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Box Collider2D Requires a Rigidbody2D to work!");
		}

		ImGuiItems::DragFloat2("Offset", circleCollider.Offset);
		ImGui::DragFloat("Radius", &circleCollider.Radius);

		ImGui::DragFloat("Density", &circleCollider.Density);
		ImGui::DragFloat("Friction", &circleCollider.Friction);
		ImGui::DragFloat("Restitution", &circleCollider.Restitution);
	}

}