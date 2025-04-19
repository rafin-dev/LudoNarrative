#include "SceneHierarchyView.h"

namespace Ludo {

	SceneHierarchyView::~SceneHierarchyView()
	{
	}

	void SceneHierarchyView::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy", 0, ImGuiWindowFlags_NoCollapse);

		if (!m_OpenAndSelecteds->GetFocusedScene())
		{
			ImGui::End();
			return;
		}

		Ref<Scene> context = AssetManager::GetAsset<Scene>(m_OpenAndSelecteds->GetFocusedScene());

		auto view = context->m_Registry.view<entt::entity>();
		for (auto entityID : view)
		{
			Entity entity(entityID, context.get());
			RenderEntityNode(entity);
		}

		if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("HierarchyOptions");
		}

		if (ImGui::BeginPopup("HierarchyOptions"))
		{
			if (ImGui::MenuItem("Add Entity"))
			{
				m_OpenAndSelecteds->SetFocusedEntity(context->CreateEntity());

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (ImGui::IsKeyDown(ImGuiKey_S) && (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)))
		{
			SceneSerializer serializer(context);
			serializer.SerializeToYamlFile(Project::GetAssetDirectory() / AssetImporter::GetAssetMetadata(m_OpenAndSelecteds->GetFocusedScene().GetUUID()).RawFilePath);
		}

		ImGui::End();
	}

	void SceneHierarchyView::RenderEntityNode(Entity entity)
	{
		ImGuiTreeNodeFlags flags = ((m_OpenAndSelecteds->GetFocusedEntity() == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool destroyEntity = false;
		bool open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, entity.GetComponent<TagComponent>().Tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_OpenAndSelecteds->SetFocusedEntity(entity);
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
			AssetManager::GetAsset<Scene>(m_OpenAndSelecteds->GetFocusedScene())->DestroyEntity(entity);
			if (m_OpenAndSelecteds->GetFocusedEntity() == entity)
			{
				m_OpenAndSelecteds->SetFocusedEntity(Entity());
			}
		}
	}

}