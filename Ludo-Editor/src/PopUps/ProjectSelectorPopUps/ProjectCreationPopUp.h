#pragma once

#include "PopUps/PopUpWindow.h"

#include <functional>

namespace Ludo {

	struct ProjectData;

	class ProjectCreationPopUp : public PopUpWindow
	{
	public:
		ProjectCreationPopUp(const std::function<void(ProjectData)>& createPj);

		void OnImGuiRender() override;

		bool ShouldClose() override { return !m_Open; }

	private:
		char m_ProjectCreationName[256] = {};
		char m_ProjectCreationFolder[256] = {};
		std::function<void(ProjectData)> m_CreatePj;

		bool m_Open = true;
	};

}