#pragma once

#include "Ludo/Core.h"
#include "Ludo/Layer.h"

namespace Ludo {

	class LUDO_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();

		void begin();
		void End();

		void OnImGuiRender() override;

	private:
	};

}