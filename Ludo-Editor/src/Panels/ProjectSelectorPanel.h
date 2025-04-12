#pragma once
#include "PanelBase.h"

#include <string>
#include <filesystem>
#include <chrono>

namespace Ludo {

	struct ProjectData
	{
		std::string Name;
		std::filesystem::path ProjectPath;
		std::chrono::time_point<std::chrono::system_clock> LastOpened;
	};

	class ProjectSelectorPanel : public PanelBase
	{
	public:
		ProjectSelectorPanel();

		void OnAttach() override;
		void OnDetach() override;

		void OnImGui() override;

	private:
		void LoadProjectList();
		void SaveProjectList();

		void RenderProjectListSelector();
		void RenderProjectOptions();

		bool m_ShoudlSetColunmWidth = true;

		std::vector<ProjectData> m_ProjectList;
		ProjectData* m_SelectedProject = nullptr;

		struct ProjectSort
		{
			bool operator() (const ProjectData& a, const ProjectData& b) const { return a.LastOpened > b.LastOpened; }
		};
	};

}