#include "FolderCreationPopup.h"

namespace Ludo {

	void FolderCreationPopUp::OnImGuiRender()
	{
		ImGui::Begin("New Folder");

		ImGui::InputText("##folderName", m_TextBuffer, sizeof(m_TextBuffer), ImGuiInputTextFlags_None);

		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
		{
			ImGui::SetKeyboardFocusHere();
		}

		ImGui::SameLine();
		if (ImGui::Button("Create Folder") || ImGui::IsKeyPressed(ImGuiKey_Enter))
		{
			m_ShouldClose = true;
			std::filesystem::create_directories(m_CurrentPath / m_TextBuffer);
		}

		if ((!ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) || ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			m_ShouldClose = true;
		}

		ImGui::End();
	}

}