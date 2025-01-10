#pragma once

#include "Core.h"

#include "Ludo/Events/KeyEvent.h"

namespace Ludo {

	class Input 
	{
	public:
		static bool IsKeyPressed(int keyCode);
		static bool IsMouseButtonDown(int buttonCode);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};

}