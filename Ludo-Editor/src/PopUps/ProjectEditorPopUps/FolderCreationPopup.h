#pragma once

#include "PopUps/PopUpWindow.h"

#include <filesystem>

namespace Ludo {

	class FolderCreationPopUp : public PopUpWindow
	{
	public:
		FolderCreationPopUp(const std::filesystem::path& currentFolder)
			: m_CurrentPath(currentFolder) { }

		void OnImGuiRender() override;

		bool ShouldClose() override { return m_ShouldClose; }

	private:
		bool m_ShouldClose = false;

		char m_TextBuffer[256] = {};

		std::filesystem::path m_CurrentPath;
	};

}