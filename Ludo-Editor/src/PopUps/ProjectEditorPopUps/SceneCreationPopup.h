#pragma once

#include "PopUps/PopUpWindow.h"

#include "LudoNarrative.h"

#include <filesystem>

namespace Ludo {

	class SceneCreationPopup : public PopUpWindow
	{
	public:
		SceneCreationPopup(const std::filesystem::path& currentPath)
			: m_CurrentPath(currentPath) { }

		void OnImGuiRender() override;

		bool ShouldClose() override { return m_ShouldClose; }

	private:
		bool m_ShouldClose = false;
		std::filesystem::path m_CurrentPath;

		char m_SceneName[256] = {};
	};

}