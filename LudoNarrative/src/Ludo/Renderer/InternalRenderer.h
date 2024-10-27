#pragma once

#include "Ludo/Core.h"

namespace Ludo {

	class 
		InternalRenderer
	{
	public:
		virtual ~InternalRenderer() {}

		virtual bool Init() = 0;

		virtual void BeginImGui() = 0;
		virtual void EndImGui() = 0;

		static InternalRenderer* Get();
	};

}