#include "InputItems.h"

#include <imgui_internal.h>

namespace Ludo::ImGuiItems {

	void DragFloat2(const std::string& label, DirectX::XMFLOAT2* values, const DirectX::XMFLOAT2& resetValues, float columnWidth)
	{
		uint32_t id = (uint32_t)label.c_str();

		ImGui::PushID(id++);
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::PopID();

		ImGui::Text(label.c_str());

		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y + 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });

		ImGui::PushID(id++);
		if (ImGui::Button("X", buttonSize))
		{
			values->x = resetValues.x;
		}
		ImGui::PopID();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushID(id++);
		ImGui::DragFloat("##x", &values->x, 1.0f, 0.0f, 0.0f, "%.2f");
		ImGui::PopID();
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });

		ImGui::PushID(id++);
		if (ImGui::Button("Y", buttonSize))
		{
			values->y = resetValues.y;
		}
		ImGui::PopID();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushID(id++);
		ImGui::DragFloat("##y", &values->y, 1.0f, 0.0f, 0.0f, "%.2f");
		ImGui::PopID();
		ImGui::PopStyleVar();
		ImGui::PopItemWidth();
		ImGui::Columns(1);
	}

	void DragFloat3(const std::string& label, DirectX::XMFLOAT3* values, const DirectX::XMFLOAT3& resetValues, float columnWidth)
	{
		uint32_t id = (uint32_t)label.c_str();

		ImGui::PushID(id++);
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::PopID();

		ImGui::Text(label.c_str());

		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y + 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });

		ImGui::PushID(id++);
		if (ImGui::Button("X", buttonSize))
		{
			values->x = resetValues.x;
		}
		ImGui::PopID();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushID(id++);
		ImGui::DragFloat("##x", &values->x, 1.0f, 0.0f, 0.0f, "%.2f");
		ImGui::PopID();
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });

		ImGui::PushID(id++);
		if (ImGui::Button("Y", buttonSize))
		{
			values->y = resetValues.y;
		}
		ImGui::PopID();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushID(id++);
		ImGui::DragFloat("##y", &values->y, 1.0f, 0.0f, 0.0f, "%.2f");
		ImGui::PopID();
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.25f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });

		ImGui::PushID(id++);
		if (ImGui::Button("Z", buttonSize))
		{
			values->z = resetValues.z;
		}
		ImGui::PopID();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::PushID(id++);
		ImGui::DragFloat("##z", &values->z, 1.0f, 0.0f, 0.0f, "%.2f");
		ImGui::PopID();
		ImGui::PopStyleVar();
		ImGui::PopItemWidth();
		ImGui::Columns(1);
	}
}