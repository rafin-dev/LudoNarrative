#include "SceneCreationPopup.h"

namespace Ludo {

	void SceneCreationPopup::OnImGuiRender()
	{
		ImGui::Begin("New Scene");

		ImGui::InputText("##sceneName", m_SceneName, sizeof(m_SceneName));

		ImGui::SameLine();

		if (ImGui::Button("Create Scene") || ImGui::IsKeyPressed(ImGuiKey_Enter))
		{
			m_ShouldClose = true;

			std::filesystem::path scenePath = m_CurrentPath / (std::string(m_SceneName) + ".ldScene");

			SceneSerializer::CreateEmptySceneAt(scenePath);

			AssetMetadata scenemtdtd = AssetImporter::CreateNewSceneMetadata();
			scenemtdtd.SceneData.StartEntityCount = 0;
			scenemtdtd.RawFilePath = std::filesystem::relative(scenePath, Project::GetAssetDirectory());

			AssetImporter::ImportAsset(scenemtdtd);
		}

		if ((!ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) || ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			m_ShouldClose = true;
		}

		ImGui::End();
	}

}