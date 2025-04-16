#include "ContentBrowserView.h"

#include "EditorApplication.h"
#include "PopUps/ProjectEditorPopUps/SceneCreationPopup.h"

namespace Ludo {

	ContentBrowserView::ContentBrowserView()
	{
		Ref<Texture2D> dirIcon = Texture2D::Create("Assets/icons/ContentBrowser/DirectoryIcon.png");
		Ref<Texture2D> fileIcon = Texture2D::Create("Assets/icons/ContentBrowser/FileIcon.png");

		m_Types =
		{
			{ std::string(), EntryType::Directory },
			{ ".ldScene", EntryType::Scene },
			{ ".png", EntryType::Texture2D }
		};

		m_Icons =
		{
			{ EntryType::Directory, dirIcon },
			{ EntryType::Scene, fileIcon },
			{ EntryType::Texture2D, fileIcon }
		};
	}

	ContentBrowserView::~ContentBrowserView()
	{
	}

	void ContentBrowserView::OnImGuiRender()
	{
		if (m_AssetDirectory != Project::GetAssetDirectory())
		{
			m_AssetDirectory = Project::GetAssetDirectory();
			m_CurrentDirectory = m_AssetDirectory;
		}

		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != m_AssetDirectory)
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		float panelWdith = ImGui::GetContentRegionAvail().x;
		ImGui::Columns(std::max((int)panelWdith / 144, 1), 0, false);

		for (auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			std::filesystem::path entryPath = entry.path();
			std::string filename = entryPath.filename().string();
			
			auto ite = m_Types.find(entryPath.extension().string());
			if (ite == m_Types.end())
			{
				continue;
			}
			auto icon = m_Icons[ite->second];

			ImGui::PushID(filename.c_str());

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::ImageButton("#", icon->GetImTextureID(), ImVec2(128.0f, 128.0f), { 0, 1 }, { 1, 0 });
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				switch (ite->second)
				{
				case EntryType::Directory:
					m_CurrentDirectory = entryPath;
					break;
				}
			}
			ImGui::PopStyleColor();

			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = entryPath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
			}
			ImGui::TextWrapped(filename.c_str());

			ImGui::PopID();

			ImGui::NextColumn();
		}

		if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("Content Browser Options");
		}

		if (ImGui::BeginPopup("Content Browser Options"))
		{
			if (ImGui::MenuItem("New Scene"))
			{
				EditorApplication::CreatePopUp<SceneCreationPopup>(m_CurrentDirectory);

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::End();
	}

}