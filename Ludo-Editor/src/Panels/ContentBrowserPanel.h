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
		Ref<ImGuiTexture>& PickIconForEntry(const std::filesystem::path& path);

		std::filesystem::path m_RootAssetsDirectory = "assets";
		std::filesystem::path m_CurrentDirectory;

		Ref<ImGuiTexture> m_FilledDirectoryIcon;
		Ref<ImGuiTexture> m_TextFileIcon;
	};

}