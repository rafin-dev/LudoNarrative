#pragma once

#include <LudoNarrative.h>

namespace Ludo::ImGuiItems {

	void DragFloat2(const std::string& label, DirectX::XMFLOAT2& values, float resetValue = 0.0f, float coloumnWidth = 100);

	void DragFloat3(const std::string& label, DirectX::XMFLOAT3& values, float resetValue = 0.0f, float coloumnWidth = 100);

}