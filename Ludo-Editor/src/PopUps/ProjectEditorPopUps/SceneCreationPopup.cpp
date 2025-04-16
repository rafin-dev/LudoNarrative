#include "SceneCreationPopup.h"

namespace Ludo {

	void SceneCreationPopup::OnImGuiRender()
	{
		ImGui::Begin("New Project");

		ImGui::Text("Scene Name: ");
		ImGui::SameLine();
		ImGui::InputText("##sceneName", m_SceneName, sizeof(m_SceneName));

		if (ImGui::Button("Create Scene"))
		{
			m_ShouldClose = true;

			std::filesystem::path scenePath = m_CurrentPath / (std::string(m_SceneName) + ".ldScene");

			SceneSerializer::CreateEmptySceneAt(scenePath);

			AssetMetadata scenemtdtd = AssetImporter::CreateNewSceneMetadata();
			scenemtdtd.RawFilePath = scenePath;

			AssetImporter::ImportAsset(scenemtdtd);
		}

		ImGui::End();
	}

}