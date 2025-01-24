#include "SceneHierarchyPanel.h"

#include "Style/ImGuiFontManager.h"

namespace Ludo {

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_SelectedEntity = Entity();
		m_Context = context;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		auto view = m_Context->m_Registry.view<entt::entity>();
		for (entt::entity entityID : view)
		{
			Entity entity(entityID, m_Context.get());
			DrawEntityNode(entity);
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
		{
			m_SelectedEntity = Entity();
		}

		// Right click on balnk space
		if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				m_SelectedEntity = m_Context->CreateEntity("Empty Entity");
			}

			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");

		if (m_SelectedEntity)
		{
			DrawComponents(m_SelectedEntity);
		}
		
		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectedEntity = entity;
		if (!m_SelectedEntity)
		{
			return;
		}

		if (m_SelectedEntity.HasComponent<SpriteRendererComponent>())
		{
			auto& sprite = entity.GetComponent<SpriteRendererComponent>();
			if (sprite.SpriteTexture)
			{
				m_SelectedEntityImGuiTexture = ImGuiTexture::Create(sprite.SpriteTexture);
			}
			else
			{
				m_SelectedEntityImGuiTexture = nullptr;
			}
		}
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool destroyEntity = false;
		bool open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, entity.GetComponent<TagComponent>().Tag.c_str());
		if (ImGui::IsItemClicked())
		{
			SetSelectedEntity(entity);
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				destroyEntity = true;
			}

			ImGui::EndPopup();
		}

		if (open)
		{
			ImGui::TreePop();
		}

		if (destroyEntity)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectedEntity == entity)
			{
				SetSelectedEntity(Entity());
			}
		}
	}

	static void DrawVec3Control(const std::string& label, DirectX::XMFLOAT3& values, float resetValue = 0.0f, float coloumnWidth = 100)
	{
		auto fontManager = ImGuiFontManager::Get();

		auto labelHash = std::hash<std::string>{}(label);
		ImGui::PushID(labelHash++);
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, coloumnWidth);
		ImGui::PopID();

		ImGui::Text(label.c_str());

		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 0.0f });

		float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		fontManager->PushDefaultFontBoldStyle();
		ImGui::PushID(labelHash++);
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
		}
		ImGui::PopID();
		fontManager->PopDefaultFontStyle();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();

		ImGui::PushID(labelHash++);
		ImGui::DragFloat("##x", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopID();
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
		fontManager->PushDefaultFontBoldStyle();
		ImGui::PushID(labelHash++);
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
		}
		ImGui::PopID();
		fontManager->PopDefaultFontStyle();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();

		ImGui::PushID(labelHash++);
		ImGui::DragFloat("##y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopID();
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.25f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		fontManager->PushDefaultFontBoldStyle();
		ImGui::PushID(labelHash++);
		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
		}
		ImGui::PopID();
		fontManager->PopDefaultFontStyle();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();

		ImGui::PushID(labelHash++);
		ImGui::DragFloat("##z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopID();
		ImGui::PopItemWidth();

		ImGui::PopStyleVar(1);
		ImGui::Columns(1);
	}


	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());

			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
			ImGui::SameLine();
			ImGui::PushItemWidth(-1);
			if (ImGui::Button("Add Component"))
			{
				ImGui::OpenPopup("AddComponent");
			}

			if (ImGui::BeginPopup("AddComponent"))
			{
				if (ImGui::MenuItem("Camera Component", nullptr, false, !m_SelectedEntity.HasComponent<CameraComponent>()))
				{
					m_SelectedEntity.AddComponent<CameraComponent>();

					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Sprite Renderer", nullptr, false, !m_SelectedEntity.HasComponent<SpriteRendererComponent>()))
				{
					m_SelectedEntity.AddComponent<SpriteRendererComponent>();

					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
			ImGui::PopItemWidth();
		}

		DrawComponent<TransformComponent>(entity, "Transform", false, true, [](Entity entity, TransformComponent& transform, auto*)
			{
				DrawVec3Control("Position", transform.Translation);

				DirectX::XMFLOAT3 rotation;
				rotation.x = DirectX::XMConvertToDegrees(transform.Rotation.x);
				rotation.y = DirectX::XMConvertToDegrees(transform.Rotation.y);
				rotation.z = DirectX::XMConvertToDegrees(transform.Rotation.z);
				DrawVec3Control("Rotation", rotation);
				transform.Rotation.x = DirectX::XMConvertToRadians(rotation.x);
				transform.Rotation.y = DirectX::XMConvertToRadians(rotation.y);
				transform.Rotation.z = DirectX::XMConvertToRadians(rotation.z);

				DrawVec3Control("Scale", transform.Scale, 1.0f);

				ImGui::Spacing();
			});

		DrawComponent<CameraComponent>(entity, "Camera", true, true, [](Entity entity, CameraComponent& cameraComponent, auto*) 
		{
			auto& camera = cameraComponent.Camera;

			ImGui::Checkbox("Primary", &cameraComponent.Primary);

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
		});

		DrawComponent<SpriteRendererComponent>(entity, "Sprite Renderer", true, true, [](Entity entity, SpriteRendererComponent& spriteComponent, SceneHierarchyPanel* panel)
		{
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), spriteComponent.SpriteTexturePath.string().c_str());
			if (ImGui::InputText("Texture Path", buffer, 256))
			{
				panel->SetSelectedEntityTexture(buffer);
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					panel->SetSelectedEntityTexture(path);
				}
				ImGui::EndDragDropTarget();
			}

			auto width = ImGui::GetContentRegionAvail().x / 2;
			if (panel->GetSelectedEntityImGuiTexture())
			{
				ImGui::SetCursorPosX((ImGui::GetWindowSize().x - width) * 0.5f);
				auto cursorPos = ImGui::GetCursorPos();
				ImGui::ColorButton("##IBG", ImVec4(0.1f, 0.1f, 0.1f, 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2{ width, width });

				ImGui::SetNextItemAllowOverlap();
				ImGui::SetCursorPos(cursorPos);
				ImGui::Image(panel->GetSelectedEntityImGuiTexture()->GetImTextureID(), { width, width }, ImVec2(0, 0), ImVec2(1, 1), 
					ImVec4(spriteComponent.SpriteColor.x, spriteComponent.SpriteColor.y, spriteComponent.SpriteColor.z, spriteComponent.SpriteColor.w));
			}

			ImGui::ColorEdit4("Color", (float*)&spriteComponent.SpriteColor);
			ImGui::InputFloat("Tilign Factor", &spriteComponent.SpriteTilingFactor);
		});

		ImGui::Separator();
	}

	void SceneHierarchyPanel::SetSelectedEntityTexture(const std::filesystem::path& path)
	{
		if (std::filesystem::exists(path))
		{
			auto& sprite = m_SelectedEntity.GetComponent<SpriteRendererComponent>();
			sprite.SpriteTexturePath = path;
			sprite.SpriteTexture = Texture2D::Create(path);
			m_SelectedEntityImGuiTexture = ImGuiTexture::Create(sprite.SpriteTexture);
		}
	}

}