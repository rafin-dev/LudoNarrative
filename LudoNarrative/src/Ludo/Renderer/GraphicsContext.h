#pragma once

#include "Ludo/Window.h"

namespace Ludo {

	class GraphicsContext
	{
	public:
		virtual bool Init() = 0;
		virtual ~GraphicsContext() = default;
		virtual void SwapBuffers() = 0;

		virtual void Resize(unsigned int width, unsigned int height) = 0;

		static GraphicsContext* Create(const Window* window);
	};

}