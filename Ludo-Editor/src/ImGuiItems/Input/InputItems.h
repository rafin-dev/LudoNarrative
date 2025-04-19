#pragma once

#include <LudoNarrative.h>

#include <imgui.h>

namespace Ludo::ImGuiItems {

	void DragFloat2(const std::string& label, DirectX::XMFLOAT2* values, const DirectX::XMFLOAT2& resetValues = {}, float columnWidth = 100.0f);
	void DragFloat3(const std::string& label, DirectX::XMFLOAT3* values, const DirectX::XMFLOAT3& resetValues = {}, float columnWidth = 100.0f);

}