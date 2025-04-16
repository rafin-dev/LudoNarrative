#include "EditorApplication.h"

#include "Panels/ProjectSelectorPanel.h"
#include "Panels/ProjectEditorPanel.h"

#include <Ludo/Core/EntryPoint.h>

namespace Ludo {

	static EditorApplication* s_app = nullptr;

	void EditorApplication::OnAttach()
	{
		AssetManager::InitEditor();

		m_Panels.emplace(PanelCodes::ProjectSelector, CreateScope<ProjectSelectorPanel>());
		m_Panels.emplace(PanelCodes::ProjectEditor, CreateScope<ProjectEditorPanel>());

		m_Panels[m_CurrentPanel]->OnAttach();

		s_app = this;
	
		GetWindow().SetSize(1600, 900); // Change size after initializing the panels to prevent an early event trigger
	}

	void EditorApplication::OnDetach()
	{
		m_Panels[m_CurrentPanel]->OnDetach();

		m_Panels.clear(); // Make sure everything gets released before the Rendering system shuts down
	}

	void EditorApplication::OnUpdate(TimeStep time)
	{
		m_Panels[m_CurrentPanel]->OnUpdate(time);

		for (auto popup = m_PopUps.begin(); popup != m_PopUps.end();)
		{
			popup->get()->OnUpdate(time);
			
			auto pp = popup;
			popup++;

			if (pp->get()->ShouldClose())
			{
				m_PopUps.erase(pp);
			}
		}
	}

	void EditorApplication::OnImGuiRender()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		float ogMinWinWidth = style.WindowMinSize.x;
		style.WindowMinSize.x = 340.0f;
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

		m_Panels[m_CurrentPanel]->OnImGui();

		for (auto& popup : m_PopUps)
		{
			popup->OnImGuiRender();
		}
	}

	void EditorApplication::OnEvent(Event& event)
	{
		m_Panels[m_CurrentPanel]->OnEvent(event);

		for (auto& popup : m_PopUps)
		{
			popup->OnEvent(event);
		}
	}

	void EditorApplication::SetPanel(PanelCodes newPanel)
	{
		s_app->SetPanelImpl(newPanel);
	}

	void EditorApplication::SetPanelImpl(PanelCodes newPanel)
	{
		if (m_Panels[m_CurrentPanel]->OnPanelChange(newPanel)) // Chekc if the current panel allows a panel change
		{
			m_Panels[m_CurrentPanel]->OnDetach();
			
			m_CurrentPanel = newPanel;

			m_Panels[m_CurrentPanel]->OnAttach();
		}
	}

	// Entry point
	Application* CreateApplication()
	{
		return new EditorApplication();
	}

}