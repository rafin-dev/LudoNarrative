#pragma once

#include <LudoNarrative.h>

#include "SceneEditorPanel.h"

namespace Ludo {

	class ScenePanelHierarchy : public SceneEditorPanel
	{
	public:
		ScenePanelHierarchy();
		~ScenePanelHierarchy() = default;

		void OnImGuiRender() override;

		void SetContext(const Ref<Scene>& context) override 
		{ 
			m_Context = context;
			SetSelectedEntity(Entity());
		}
		const Ref<Scene>& GetContext() const override { return m_Context; }

		Entity GetSelectedEntity() { return m_SelectedEntity; }
		void SetSelectedEntity(Entity entity);

		void SetSelectedEntityTexture(const std::filesystem::path& path);

		bool IsActive() const override { return m_IsActive; }
		void SetActive(bool active) override { m_IsActive = active; }

	private:
		void RenderHierarchyPanel();
		void RenderPropertiesPanel();
		
		void RenderEntityNode(Entity entity);
		void RenderEntityTagAndComponentOptions();

		bool m_IsActive = true;

		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
		Ref<ImGuiTexture> m_SelectedEntityImGuiTexture;
		
		std::unordered_map<size_t, std::function<void()>> m_ComponentPanelRenderers;

		void OnRenderTransformComponent();
		void OnRenderSpriteRendererComponent();
		void OnRenderCircleRendererComponent();
		void OnRenderCameraComponent();
		void OnRenderRigidbody2DComponent();
		void OnRenderBoxCollider2DComponent();

		template<typename T>
		void RenderComponentPropertiesIfExists(const std::string& label, bool removable = true)
		{
			if (m_SelectedEntity.HasComponent<T>())
			{
				bool removeComponent = false;
				ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4.0f, 4.0f });
				ImGui::Separator();

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Selected;
				flags |= ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
				flags |= ImGuiTreeNodeFlags_DefaultOpen; 
				bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, label.c_str());

				float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
				ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);
				if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
				{
					ImGui::OpenPopup("ComponentSettings");
				}
				ImGui::PopStyleVar();

				if (ImGui::BeginPopup("ComponentSettings"))
				{
					if (ImGui::MenuItem("Remove Component", nullptr, false, removable)) { removeComponent = true; }

					ImGui::EndPopup();
				}

				if (open)
				{
					m_ComponentPanelRenderers[typeid(T).hash_code()]();
					ImGui::TreePop();
				}

				if (removeComponent)
				{
					m_SelectedEntity.RemoveComponent<T>();
				}
			}
		}
	};

}