#pragma once

#include "Ludo/Core/Window.h"

namespace Ludo {

	class GraphicsContext
	{
	public:
		virtual bool Init() = 0;
		virtual ~GraphicsContext() = default;
		virtual void SwapBuffers() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		static GraphicsContext* Create(const Window* window);
	};

}