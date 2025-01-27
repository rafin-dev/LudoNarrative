#pragma once

#include <LudoNarrative.h>

#include <imgui_internal.h>

namespace Ludo {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);
		const Ref<Scene>& GetContext();

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectedEntity; }
		void SetSelectedEntity(Entity entity);

		const Ref<ImGuiTexture>& GetSelectedEntityImGuiTexture() { return m_SelectedEntityImGuiTexture; }

	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

		template<typename T>
		void DrawComponent(Entity entity, const std::string& label, bool Removable, bool DefaultOpen, void(*ImGuiRender)(Entity, T&, SceneHierarchyPanel* panel))
		{
			if (entity.HasComponent<T>())
			{
				bool removeComponent = false;
				ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4.0f, 4.0f });
				ImGui::Separator();

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Selected;
				flags |= ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
				if (DefaultOpen) { flags |= ImGuiTreeNodeFlags_DefaultOpen; }
				bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, label.c_str());

				float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
				ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);
				if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
				{
					ImGui::OpenPopup("ComponentSettings");
				}
				ImGui::PopStyleVar();

				if (ImGui::BeginPopup("ComponentSettings"))
				{
					if (ImGui::MenuItem("Remove Component", nullptr, false, Removable)) { removeComponent = true; }

					ImGui::EndPopup();
				}

				if (open)
				{
					ImGuiRender(entity, entity.GetComponent<T>(), this);
					ImGui::TreePop();
				}

				if (removeComponent)
				{
					entity.RemoveComponent<T>();
				}
			}
		}

		void SetSelectedEntityTexture(const std::filesystem::path& path);

		Ref<Scene> m_Context;

		Entity m_SelectedEntity;
		Ref<ImGuiTexture> m_SelectedEntityImGuiTexture;
	};

}