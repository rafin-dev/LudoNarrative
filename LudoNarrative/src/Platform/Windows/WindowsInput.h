#pragma once

#include "Ludo/Core/Input.h"

#include "Windows.h"

namespace Ludo {

	class WindowsInput : public Input
	{
	protected:
		bool IsKeyPressedImpl(int keyCode) override;

		bool IsMouseButtonDownImpl(int buttonCode) override;

		std::pair<float, float> GetMousePositionImpl() override;
		float GetMouseXImpl() override;
		float GetMouseYImpl() override;
	};

}