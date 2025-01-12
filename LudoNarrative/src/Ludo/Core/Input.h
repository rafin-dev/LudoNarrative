#pragma once

#include "Core.h"

#include "Ludo/Events/KeyEvent.h"
#include "Ludo/Core/KeyCodes.h"

namespace Ludo {

	class Input 
	{
	public:
		static bool IsKeyPressed(KeyCode keyCode);
		static bool IsMouseButtonDown(MouseButtonCode buttonCode);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};

}