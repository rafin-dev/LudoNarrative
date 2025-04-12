#include "ProjectCreationPopUp.h"

#include "Panels/ProjectSelectorPanel.h"

namespace Ludo {

	ProjectCreationPopUp::ProjectCreationPopUp(const std::function<void(ProjectData)>& createPj)
	{
		m_CreatePj = createPj;

		memset(m_ProjectCreationName, 0, sizeof(m_ProjectCreationName));
		strcpy_s(m_ProjectCreationName, sizeof(m_ProjectCreationName), "New Project");

		memset(m_ProjectCreationFolder, 0, sizeof(m_ProjectCreationFolder));
		strcpy_s(m_ProjectCreationFolder, sizeof(m_ProjectCreationFolder), FileDialogs::GetDocumentsFolder().string().c_str());
	}

	void ProjectCreationPopUp::OnImGuiRender()
	{
		ImGui::Begin("Project Creation", &m_Open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);
	
		ImGui::Text("Project Name");
		ImGui::SameLine(130.0f);
		ImGui::InputText("##nameInput", m_ProjectCreationName, sizeof(m_ProjectCreationName));

		ImGui::Text("Project Location");
		ImGui::SameLine(130.0f);
		ImGui::InputText("##locationInput", m_ProjectCreationFolder, sizeof(m_ProjectCreationFolder));

		ImGui::SameLine();
		if (ImGui::Button("Select Folder"))
		{
			auto path = FileDialogs::GetFolder();

			if (!path.empty())
			{
				strcpy_s(m_ProjectCreationFolder, sizeof(m_ProjectCreationFolder), path.string().c_str());
			}
		}

		if (ImGui::Button("Create Project"))
		{
			std::string name(m_ProjectCreationName);
			std::filesystem::path pjPath = m_ProjectCreationFolder / std::filesystem::path(name) / std::filesystem::path(name + ".ldProject");

			Project::New(pjPath, name);
			m_CreatePj(ProjectData{ name, pjPath, std::chrono::system_clock::now() });
			m_Open = false;
		}

		ImGui::End();
	}

}