#pragma once

#include "Ludo/Core.h"

#include <d3d9.h>

namespace Ludo {

	class LUDO_API InternalRenderer
	{
	public:
		virtual ~InternalRenderer() {}

		virtual void Init() = 0;

		virtual void BeginScene() = 0;
		virtual void EndScene() = 0;

		static InternalRenderer& Get();
	};

}