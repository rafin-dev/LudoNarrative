#pragma once

#include <LudoNarrative.h>
#include <imgui/imgui.h>

#include <filesystem>

namespace Ludo {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		
		void OnImGuiRender();

	private:
		std::filesystem::path m_RootAssetsDirectory = "assets";
		std::filesystem::path m_CurrentDirectory;
	};

}