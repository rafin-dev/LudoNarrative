#include "ProjectEditorPanel.h"

#include "ProjectEditorViews/EntityPropertiesView.h"
#include "ProjectEditorViews/SceneHierarchyView.h"
#include "ProjectEditorViews/ContentBrowserView.h"
#include "ProjectEditorViews/SceneViewerView.h"
#include "ProjectEditorViews/Status/AssetManagerStatusView.h"
#include "ProjectEditorViews/Status/AssetImporterStatusView.h"

#include "ProjectEditorViews/Misc/OpenAndSelectedsManager.h"

#include <EditorApplication.h>

namespace  Ludo {

	ProjectEditorPanel::ProjectEditorPanel()
	{
		Ref<OpenAndSelectedsManager> oas = CreateRef<OpenAndSelectedsManager>();

		m_Views.insert(std::pair(Views::EntityProperties, CreateRef<EntityPropertiesView>(oas)));
		m_Views.insert(std::pair(Views::SceneHierarchy, CreateRef<SceneHierarchyView>(oas)));
		m_Views.insert(std::pair(Views::ContentBrowser, CreateRef<ContentBrowserView>(oas)));
		m_Views.insert(std::pair(Views::SceneViewer, CreateRef<SceneViewerView>(oas)));

		m_Views.insert(std::pair(Views::AssetManagerStatus, CreateRef<AssetManagerStatusView>()));
		m_Views.insert(std::pair(Views::AssetImporterStatus, CreateRef<AssetImporterStatusView>()));

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
		for (auto& view : m_Views)
		{
			if (view.second->Active)
			{
				view.second->OnUpdate(ts);
			}
		}
	}

	void ProjectEditorPanel::OnImGui()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("Close Project"))
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
				ImGui::Checkbox("Asset Manager Status", &m_Views.at(Views::AssetManagerStatus)->Active);
				ImGui::Checkbox("Asset Importer Status", &m_Views.at(Views::AssetImporterStatus)->Active);


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
		for (auto& view : m_Views)
		{
			if (view.second->Active)
			{
				view.second->OnEvent(e);
			}
		}
	}

	bool ProjectEditorPanel::OnPanelChange(PanelCodes newPanel)
	{
		return true;
	}

}