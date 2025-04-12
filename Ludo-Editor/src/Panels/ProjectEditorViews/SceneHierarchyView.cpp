#include "SceneHierarchyView.h"

namespace Ludo {

	SceneHierarchyView::~SceneHierarchyView()
	{
	}

	void SceneHierarchyView::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy", 0, ImGuiWindowFlags_NoCollapse);

		if (m_Context)
		{
			auto view = m_Context->m_Registry.view<entt::entity>();
			for (auto entityID : view)
			{
				Entity entity(entityID, m_Context.get());
				RenderEntityNode(entity);
			}
		}

		ImGui::End();
	}

	void SceneHierarchyView::RenderEntityNode(Entity entity)
	{
		ImGuiTreeNodeFlags flags = ((m_EntityProperties->SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool destroyEntity = false;
		bool open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, entity.GetComponent<TagComponent>().Tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_EntityProperties->SelectedEntity = entity;
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
			if (m_EntityProperties->SelectedEntity == entity)
			{
				m_EntityProperties->SelectedEntity = Entity();
			}
		}
	}

}