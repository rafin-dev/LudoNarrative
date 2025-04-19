#include "EntityPropertiesView.h"

#include "PopUps/ProjectEditorPopUps/TexturePickerPopUp.h"
#include "EditorApplication.h"

#include "ImGuiItems/ImGuiItems.h"

#define COMPONENT_TYPEID_HASH(x) (typeid(x).hash_code())

#define BIND_COMPONENT_PANEL_RENDERER(x) { COMPONENT_TYPEID_HASH(x), std::bind(&EntityPropertiesView::OnRender##x, this) }

namespace Ludo {

	EntityPropertiesView::EntityPropertiesView(const Ref<OpenAndSelectedsManager>& oas)
		: m_OpenAndSelected(oas)
	{
		m_PropertieRendereres = {
			BIND_COMPONENT_PANEL_RENDERER(TransformComponent),
			BIND_COMPONENT_PANEL_RENDERER(SpriteRendererComponent),
			BIND_COMPONENT_PANEL_RENDERER(CircleRendererComponent),
			BIND_COMPONENT_PANEL_RENDERER(CameraComponent),
			BIND_COMPONENT_PANEL_RENDERER(Rigidbody2DComponent),
			BIND_COMPONENT_PANEL_RENDERER(BoxCollider2DComponent),
			BIND_COMPONENT_PANEL_RENDERER(CircleCollider2DComponent)
		};
	}

	EntityPropertiesView::~EntityPropertiesView()
	{
	}

	void EntityPropertiesView::OnImGuiRender()
	{
		ImGui::Begin("Entity Properties");

		Entity entity = m_OpenAndSelected->GetFocusedEntity();

		if (entity)
		{
			OnRenderEntityTagAndComponentOptions();

			RenderComponentPropertiesIfExists<TransformComponent, false>("Transform");
			RenderComponentPropertiesIfExists<SpriteRendererComponent>("Sprite Renderer");
			RenderComponentPropertiesIfExists<CircleRendererComponent>("Circle Renderer");
			RenderComponentPropertiesIfExists<CameraComponent>("Camera Component");
			RenderComponentPropertiesIfExists<Rigidbody2DComponent>("Rigidbody2D");
			RenderComponentPropertiesIfExists<BoxCollider2DComponent>("Box Collider2D");
			RenderComponentPropertiesIfExists<CircleCollider2DComponent>("Circle Collider2D");
		}

		ImGui::End();
	}

	template<typename T, bool Removable>
	inline void EntityPropertiesView::RenderComponentPropertiesIfExists(const std::string& label)
	{
		Entity entity = m_OpenAndSelected->GetFocusedEntity();

		if (entity.HasComponent<T>())
		{
			bool removeComponent = false;
			ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
			ImGui::Separator();

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Selected;
			flags |= ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, label.c_str());

			ImGui::PushID(label.c_str());
			float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
			ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}
			ImGui::PopStyleVar();
			ImGui::PopID();

			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component", nullptr, false, Removable)) { removeComponent = true; }

				ImGui::EndPopup();
			}

			if (open)
			{
				m_PropertieRendereres[typeid(T).hash_code()]();
				ImGui::TreePop();
			}

			if (removeComponent)
			{
				entity.RemoveComponent<T>();
			}
		}
	}

#define AddComponentButton(component) if (ImGui::MenuItem(#component, nullptr, false, !entity.HasComponent<component>())) \
		{ \
			entity.AddComponent<component>(); \
			ImGui::CloseCurrentPopup();\
		}

	void EntityPropertiesView::OnRenderEntityTagAndComponentOptions()
	{
		Entity entity = m_OpenAndSelected->GetFocusedEntity();

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
			AddComponentButton(CameraComponent);
			AddComponentButton(SpriteRendererComponent);
			AddComponentButton(CircleRendererComponent);
			AddComponentButton(Rigidbody2DComponent);
			AddComponentButton(BoxCollider2DComponent);
			AddComponentButton(CircleCollider2DComponent);

			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();
	}

	// Component Propertie Renderers
	void EntityPropertiesView::OnRenderTransformComponent()
	{
		auto& transform = m_OpenAndSelected->GetFocusedEntity().GetComponent<TransformComponent>();

		ImGuiItems::DragFloat3("Position", &transform.Translation);

		DirectX::XMFLOAT3 rotation;
		rotation.x = DirectX::XMConvertToDegrees(transform.Rotation.x);
		rotation.y = DirectX::XMConvertToDegrees(transform.Rotation.y);
		rotation.z = DirectX::XMConvertToDegrees(transform.Rotation.z);
	
		ImGuiItems::DragFloat3("Rotation", &rotation);
		transform.Rotation.x = DirectX::XMConvertToRadians(rotation.x);
		transform.Rotation.y = DirectX::XMConvertToRadians(rotation.y);
		transform.Rotation.z = DirectX::XMConvertToRadians(rotation.z);

		ImGuiItems::DragFloat3("Scale", &transform.Scale);

		ImGui::Spacing();
	}

	void EntityPropertiesView::OnRenderSpriteRendererComponent()
	{
		auto& spriteComponent = m_OpenAndSelected->GetFocusedEntity().GetComponent<SpriteRendererComponent>();

		bool createTexturePopUp = false;

		if (ImGui::Button("Set Texture"))
		{
			createTexturePopUp = true;
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;

				auto metadata = AssetImporter::GetAssetMetadata(path);
				spriteComponent.Texture = AssetManager::LoadAsset(metadata.AssetUUID);
			}
			ImGui::EndDragDropTarget();
		}

		bool removeTx = false;
		if (spriteComponent.Texture)
		{
			if (ImGui::Button("Remove Texture", { ImGui::GetContentRegionAvail().x, 0.0f }))
			{
				removeTx = true;
			}

			uint32_t width = ImGui::GetContentRegionAvail().x / 2;
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - width) * 0.5f);
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - width) * 0.5f);
			auto cursorPos = ImGui::GetCursorPos();
			ImGui::ColorButton("##IBG", ImVec4(0.1f, 0.1f, 0.1f, 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2{ (float)width, (float)width });

			ImGui::SetNextItemAllowOverlap();
			ImGui::SetCursorPos(cursorPos);
			ImGui::Image(AssetManager::GetAsset<Texture2D>(spriteComponent.Texture)->GetImTextureID(), { (float)width, (float)width}, ImVec2(0, 1), ImVec2(1, 0),
				ImVec4(spriteComponent.Color.x, spriteComponent.Color.y, spriteComponent.Color.z, spriteComponent.Color.w));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;

					auto metadata = AssetImporter::GetAssetMetadata(path);
					spriteComponent.Texture = AssetManager::LoadAsset(metadata.AssetUUID);
				}
				ImGui::EndDragDropTarget();
			}
		}

		ImGui::ColorEdit4("Color", (float*)&spriteComponent.Color);
		ImGui::DragFloat("Tilign Factor", &spriteComponent.TilingFactor);

		if (removeTx)
		{
			spriteComponent.Texture = 0;
		}

		if (createTexturePopUp)
		{
			EditorApplication::CreatePopUp<TexturePickerPopUp>(m_OpenAndSelected->GetFocusedEntity());
		}
	}

	void EntityPropertiesView::OnRenderCircleRendererComponent()
	{
		auto& circleRenderer = m_OpenAndSelected->GetFocusedEntity().GetComponent<CircleRendererComponent>();

		ImGui::ColorEdit4("Color", (float*)&circleRenderer.Color);
		ImGui::DragFloat("Thickness", &circleRenderer.Thickness, 0.025, 0.0f, 1.0f);
		ImGui::DragFloat("Fade", &circleRenderer.Fade, 0.00025, 0.0f, 1.0f);
	}

	void EntityPropertiesView::OnRenderScriptComponent()
	{
	}

	void EntityPropertiesView::OnRenderCameraComponent()
	{
		Entity entity = m_OpenAndSelected->GetFocusedEntity();

		auto& cameraComponent = entity.GetComponent<CameraComponent>();
		auto& camera = cameraComponent.Camera;

		bool isMainCamera = m_OpenAndSelected->GetFocusedEntity().IsMainCamera();
		if (ImGui::Checkbox("Primary", &isMainCamera))
		{
			if (isMainCamera)
			{
				entity.SetAsMainCamera();
			}
			else
			{
				AssetManager::GetAsset<Scene>(m_OpenAndSelected->GetFocusedScene())->SetMainCamera(Entity());
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

	void EntityPropertiesView::OnRenderRigidbody2DComponent()
	{
		auto& rigidbody = m_OpenAndSelected->GetFocusedEntity().GetComponent<Rigidbody2DComponent>();

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

	void EntityPropertiesView::OnRenderBoxCollider2DComponent()
	{
		auto& boxCollider = m_OpenAndSelected->GetFocusedEntity().GetComponent<BoxCollider2DComponent>();

		if (!m_OpenAndSelected->GetFocusedEntity().HasComponent<Rigidbody2DComponent>())
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Box Collider2D Requires a Rigidbody2D to work!");
		}

		ImGuiItems::DragFloat2("Offset", &boxCollider.Offset);
		ImGuiItems::DragFloat2("Size", &boxCollider.Size, { 1.0f, 1.0f });
		float rotation = DirectX::XMConvertToDegrees(boxCollider.Rotation);
		if (ImGui::DragFloat("Rotation", &rotation))
		{
			boxCollider.Rotation = DirectX::XMConvertToRadians(rotation);
		}

		ImGui::DragFloat("Density", &boxCollider.Density);
		ImGui::DragFloat("Friction", &boxCollider.Friction);
		ImGui::DragFloat("Restitution", &boxCollider.Restitution);
	}

	void EntityPropertiesView::OnRenderCircleCollider2DComponent()
	{
		CircleCollider2DComponent& circleCollider = m_OpenAndSelected->GetFocusedEntity().GetComponent<CircleCollider2DComponent>();

		if (!m_OpenAndSelected->GetFocusedEntity().HasComponent<Rigidbody2DComponent>())
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Box Collider2D Requires a Rigidbody2D to work!");
		}

		ImGuiItems::DragFloat2("Offset", &circleCollider.Offset);
		ImGui::DragFloat("Radius", &circleCollider.Radius);

		ImGui::DragFloat("Density", &circleCollider.Density);
		ImGui::DragFloat("Friction", &circleCollider.Friction);
		ImGui::DragFloat("Restitution", &circleCollider.Restitution);
	}

}