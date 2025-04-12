#include "ProjectSelectorPanel.h"

#include "Ludo/Utils/PlatformUtils.h"
#include "Ludo/Project/Project.h"
#include "EditorApplication.h"
#include "PopUps/ProjectSelectorPopups/ProjectCreationPopUp.h"

#include <yaml-cpp/yaml.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <fstream>
#include <sstream>
#include <ctime>

namespace Ludo {

	ProjectSelectorPanel::ProjectSelectorPanel()
	{
		LoadProjectList();
	}

	void ProjectSelectorPanel::OnAttach()
	{
	}

	void ProjectSelectorPanel::OnDetach()
	{
		SaveProjectList();
	}

	void ProjectSelectorPanel::OnImGui()
	{
		if (ImGui::BeginMainMenuBar())
		{
			ImGui::TextColored(ImVec4(0.7f, 0.5f, 0.9f, 1.0f), "LudoNarrative");

			ImGui::EndMainMenuBar();
		}

		ImGuiWindowClass wClass;
		wClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;

		ImGui::SetNextWindowClass(&wClass);
		ImGui::Begin("Projects", nullptr, ImGuiWindowFlags_NoDecoration);
		
		if (ImGui::Button("New Project"))
		{
			EditorApplication::CreatePopUp<ProjectCreationPopUp>([&](ProjectData pd)
				{
					m_ProjectList.push_back(pd);
					std::sort(m_ProjectList.begin(), m_ProjectList.end(), ProjectSort());
				});
		}

		ImGui::Separator();

		ImGui::Columns(2);
		if (m_ShoudlSetColunmWidth) { ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() - (ImGui::GetWindowWidth() / 4)); m_ShoudlSetColunmWidth = false; }

		RenderProjectListSelector();

		ImGui::NextColumn();

		RenderProjectOptions();

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
		{
			m_SelectedProject = nullptr;
		}

		ImGui::End();
	}

	void ProjectSelectorPanel::LoadProjectList()
	{
		if (!std::filesystem::exists("ProjectList.yaml"))
		{
			return;
		}

		m_ProjectList.clear();

		std::stringstream stream;
		stream << std::ifstream("ProjectList.yaml").rdbuf();

		YAML::Node data = YAML::Load(stream.str());

		uint32_t projectCount = data["ProjectCount"].as<uint32_t>();
		m_ProjectList.reserve(projectCount);

		for (auto projectData : data["Projects"])
		{
			std::string name = projectData["ProjectName"].as<std::string>();
			std::filesystem::path filePath = projectData["ProjectFilePath"].as<std::string>();
			std::chrono::time_point<std::chrono::system_clock> lastOpened = std::chrono::system_clock::from_time_t(projectData["LastTimeOpened"].as<time_t>());

			m_ProjectList.push_back({ name, filePath, lastOpened });
		}
		
		std::sort(m_ProjectList.begin(), m_ProjectList.end(), ProjectSort());
	}

	void ProjectSelectorPanel::SaveProjectList()
	{
		YAML::Emitter out;

		out << YAML::BeginMap;

		out << YAML::Key << "ProjectCount" << YAML::Value << m_ProjectList.size();

		out << YAML::Key << "Projects" << YAML::Value << YAML::BeginSeq;

		for (auto& pjData : m_ProjectList)
		{
			out << YAML::BeginMap;

			out << YAML::Key << "ProjectName" << YAML::Value << pjData.Name;

			out << YAML::Key << "ProjectFilePath" << YAML::Value << pjData.ProjectPath.string();

			out << YAML::Key << "LastTimeOpened" << YAML::Value << std::chrono::system_clock::to_time_t(pjData.LastOpened);

			out << YAML::EndMap;
		}

		out << YAML::EndSeq << YAML::EndMap;

		std::ofstream pjListFile("ProjectList.yaml");

		pjListFile << out.c_str();
	}

	void ProjectSelectorPanel::RenderProjectListSelector()
	{
		// Project List
		ImGui::BeginChild("ProjectSelector", ImVec2());
		for (auto& pjData : m_ProjectList)
		{
			auto cursorPos = ImGui::GetCursorPos();

			ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, 50.0f);

			ImGui::PushID(pjData.ProjectPath.string().c_str());
			if (ImGui::ColorButton("##Bg",
				ImVec4(0.2f, 0.2f, 0.2f, 1.0f),
				ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop,
				size))
			{
				m_SelectedProject = &pjData;
			}
			ImGui::PopID();

			bool hovered = ImGui::IsItemHovered();

			if (hovered)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
			}

			ImGui::SetCursorPos(cursorPos);
			ImGui::Spacing();

			ImGui::Text(" Name: %s", pjData.Name.c_str());
			time_t t_0 = pjData.LastOpened.time_since_epoch().count();

			std::string str = std::format("Last Modified: {:%d/%m/%Y} ", pjData.LastOpened);
			ImGui::SameLine(ImGui::GetContentRegionMax().x - ImGui::CalcTextSize(str.c_str()).x);
			ImGui::Text(str.c_str());

			ImGui::Separator();
			ImGui::Text(" Location: %s", pjData.ProjectPath.string().c_str());

			if (hovered)
			{
				ImGui::PopStyleColor();
			}

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();
		}
		ImGui::EndChild();
	}

	void ProjectSelectorPanel::RenderProjectOptions()
	{
		ImGui::BeginChild("Project Options");

		ImGui::Text(m_SelectedProject ? m_SelectedProject->Name.c_str() : "[Select or Create Project]");

		ImGui::PushStyleColor(ImGuiCol_Button, m_SelectedProject ? ImVec4(0.3f, 0.3f, 0.3f, 1.0f) : ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, m_SelectedProject ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 0.4f));

		if (ImGui::Button("Open", ImVec2(100.0f, 30.0f)))
		{
			if (m_SelectedProject != nullptr)
			{
				Project::Load(m_SelectedProject->ProjectPath);
				m_SelectedProject->LastOpened = std::chrono::system_clock::now();
				std::sort(m_ProjectList.begin(), m_ProjectList.end(), ProjectSort());
				EditorApplication::SetPanel(PanelCodes::ProjectEditor);
			}
		}
		if (ImGui::Button("Remove", ImVec2(100.0f, 30.0f)))
		{
			if (m_SelectedProject != nullptr)
			{
				uint32_t index = ((uint32_t)(m_SelectedProject - m_ProjectList.data())) / sizeof(ProjectData);
				m_ProjectList.erase(m_ProjectList.begin() + index);
				m_SelectedProject = nullptr;
			}
		}

		ImGui::PopStyleColor(2);

		ImGui::EndChild();
	}

}