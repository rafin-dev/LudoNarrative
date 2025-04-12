#include "ProjectEditorPanel.h"

#include "ProjectEditorViews/EntityPropertiesView.h"
#include "ProjectEditorViews/SceneHierarchyView.h"

#include <EditorApplication.h>

namespace  Ludo {

	ProjectEditorPanel::ProjectEditorPanel()
	{
		m_Views.insert(std::pair(Views::EntityProperties, CreateRef<EntityPropertiesView>()));
		m_Views.insert(std::pair(Views::SceneHierarchy, CreateRef<SceneHierarchyView>(std::static_pointer_cast<EntityPropertiesView>(m_Views.at(Views::EntityProperties)))));
	}

	ProjectEditorPanel::~ProjectEditorPanel()
	{
	}

	void ProjectEditorPanel::OnAttach()
	{
	}

	void ProjectEditorPanel::OnDetach()
	{
	}

	void ProjectEditorPanel::OnUpdate(TimeStep ts)
	{
	}

	void ProjectEditorPanel::OnImGui()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("Open Project"))
				{
					EditorApplication::SetPanel(PanelCodes::ProjectSelector);
				}
				
				if (ImGui::MenuItem("Exit"))
				{
					Application::Get().Close();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Views"))
			{
				ImGui::SeparatorText("Scene");

				ImGui::Checkbox("Entity Properties", &m_Views.at(Views::EntityProperties)->Active);
				ImGui::Checkbox("Scene Hierarchy", &m_Views.at(Views::SceneHierarchy)->Active);

				ImGui::SeparatorText("Assets");

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		for (auto& view : m_Views)
		{
			if (view.second->Active)
			{
				view.second->OnImGuiRender();
			}
		}
	}

	void ProjectEditorPanel::OnEvent(Event& e)
	{
	}

	bool ProjectEditorPanel::OnPanelChange(PanelCodes newPanel)
	{
		return true;
	}

}