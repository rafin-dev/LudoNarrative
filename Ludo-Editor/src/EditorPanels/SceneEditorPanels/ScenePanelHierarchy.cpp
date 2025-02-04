#include "ScenePanelHierarchy.h"

#define COMPONENT_TYPEID_HASH(x) (typeid(x).hash_code())

#define BIND_COMPONENT_PANEL_RENDERER(x) { COMPONENT_TYPEID_HASH(x), std::bind(&ScenePanelHierarchy::OnRender##x, this) }

namespace Ludo {

	ScenePanelHierarchy::ScenePanelHierarchy()
	{
		m_ComponentPanelRenderers = {
			BIND_COMPONENT_PANEL_RENDERER(TransformComponent),
			BIND_COMPONENT_PANEL_RENDERER(SpriteRendererComponent),
			BIND_COMPONENT_PANEL_RENDERER(CircleRendererComponent),
			BIND_COMPONENT_PANEL_RENDERER(CameraComponent),
			BIND_COMPONENT_PANEL_RENDERER(Rigidbody2DComponent),
			BIND_COMPONENT_PANEL_RENDERER(BoxCollider2DComponent)
		};
	}

	void ScenePanelHierarchy::OnImGuiRender()
	{
		RenderHierarchyPanel();

		RenderPropertiesPanel();
	}

	void ScenePanelHierarchy::SetSelectedEntity(Entity entity)
	{
		m_SelectedEntity = entity;

		if (m_SelectedEntity)
		{
			if (m_SelectedEntity.HasComponent<SpriteRendererComponent>())
			{
				auto& sprite = m_SelectedEntity.GetComponent<SpriteRendererComponent>();

				if (sprite.Texture)
				{
					m_SelectedEntityImGuiTexture = ImGuiTexture::Create(sprite.Texture);
					return;
				}
			}
		}

		m_SelectedEntityImGuiTexture = nullptr;
	}

	void ScenePanelHierarchy::SetSelectedEntityTexture(const std::filesystem::path& path)
	{
		if (std::filesystem::exists(path))
		{
			auto& sprite = m_SelectedEntity.GetComponent<SpriteRendererComponent>();

			sprite.TexturePath = path;
			sprite.Texture = SubTexture2D::Create(Texture2D::Create(path));
			m_SelectedEntityImGuiTexture = ImGuiTexture::Create(sprite.Texture);
		}
	}

	void ScenePanelHierarchy::RenderHierarchyPanel()
	{
		ImGui::Begin("Scene Hierarchy");

		auto view = m_Context->m_Registry.view<entt::entity>();
		for (auto entityID : view)
		{
			Entity entity(entityID, m_Context.get());
			RenderEntityNode(entity);
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
		{
			m_SelectedEntity = Entity();
		}

		// Right click on blank space
		if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				SetSelectedEntity(m_Context->CreateEntity("Empty Entity"));
			}

			ImGui::EndPopup();
		}

		ImGui::End();

	}

	void ScenePanelHierarchy::RenderPropertiesPanel()
	{
		ImGui::Begin("Properties");

		if (m_SelectedEntity)
		{
			RenderEntityTagAndComponentOptions();

			if (m_SelectedEntity)
			{
				RenderComponentPropertiesIfExists<TransformComponent>("Transform", false);
				RenderComponentPropertiesIfExists<SpriteRendererComponent>("Sprite Renderer");
				RenderComponentPropertiesIfExists<CircleRendererComponent>("Circle Renderer");
				RenderComponentPropertiesIfExists<CameraComponent>("Camera Component");
				RenderComponentPropertiesIfExists<Rigidbody2DComponent>("Rigidbody2D");
				RenderComponentPropertiesIfExists<BoxCollider2DComponent>("Box Collider2D");
			}
		}

		ImGui::Separator();
		ImGui::End();
	}

	void ScenePanelHierarchy::RenderEntityNode(Entity entity)
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

	void ScenePanelHierarchy::RenderEntityTagAndComponentOptions()
	{
		auto& tag = m_SelectedEntity.GetComponent<TagComponent>().Tag;
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

			if (ImGui::MenuItem("Circle Renderer", nullptr, false, !m_SelectedEntity.HasComponent<CircleRendererComponent>()))
			{
				m_SelectedEntity.AddComponent<CircleRendererComponent>();

				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Rigidbody2D", nullptr, false, !m_SelectedEntity.HasComponent<Rigidbody2DComponent>()))
			{
				m_SelectedEntity.AddComponent<Rigidbody2DComponent>();

				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Box Collider2D", nullptr, false, !m_SelectedEntity.HasComponent<BoxCollider2DComponent>()))
			{
				m_SelectedEntity.AddComponent<BoxCollider2DComponent>();

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();
	}

}