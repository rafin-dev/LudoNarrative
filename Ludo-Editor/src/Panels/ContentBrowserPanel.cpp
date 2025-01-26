#include "ContentBrowserPanel.h"

namespace Ludo {

	ContentBrowserPanel::ContentBrowserPanel()
	{
		m_CurrentDirectory = m_RootAssetsDirectory;
	
		m_FilledDirectoryIcon = ImGuiTexture::Create(Texture2D::Create("assets/icons/contentBrowser/DirectoryIcon.png"));
		m_TextFileIcon = ImGuiTexture::Create(Texture2D::Create("assets/icons/contentBrowser/FileIcon.png"));
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

		float panelWidth = ImGui::GetContentRegionAvail().x;
		ImGui::Columns(std::max((int)panelWidth / 144, 1), 0, false);

		for (auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			std::filesystem::path entryPath = entry.path();
			std::string fileName = entryPath.filename().string();

			ImGui::PushID(fileName.c_str());
			auto& icon = PickIconForEntry(entryPath);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::ImageButton("##", icon->GetImTextureID(), ImVec2{ 128.0f, 128.0f }, { 0, 1 }, { 1, 0 });
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (entry.is_directory())
				{
					m_CurrentDirectory = entryPath;
				}
			}
			ImGui::PopStyleColor();

			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = entryPath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}
			ImGui::TextWrapped(fileName.c_str());

			ImGui::PopID();

			ImGui::NextColumn();
		}

		ImGui::Columns(1);

		ImGui::End();
	}

	Ref<ImGuiTexture>& ContentBrowserPanel::PickIconForEntry(const std::filesystem::path& path)
	{
		if (std::filesystem::is_directory(path))
		{
			return m_FilledDirectoryIcon;
		}
		else
		{
			return m_TextFileIcon;
		}
	}

}