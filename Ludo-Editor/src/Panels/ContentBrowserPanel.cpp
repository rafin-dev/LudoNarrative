#include "ContentBrowserPanel.h"

namespace Ludo {

	ContentBrowserPanel::ContentBrowserPanel()
	{
		m_CurrentDirectory = m_RootAssetsDirectory;
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != m_RootAssetsDirectory)
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		for (auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			std::filesystem::path entryPath = entry.path();

			if (ImGui::Button(entryPath.filename().string().c_str()))
			{
				if (entry.is_directory())
				{
					m_CurrentDirectory = entryPath;
				}
			}
		}

		ImGui::End();
	}

}