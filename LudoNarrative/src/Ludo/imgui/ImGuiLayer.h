#pragma once

#include "Ludo/Core.h"
#include "Ludo/Layer.h"

namespace Ludo {

	class LUDO_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();

		virtual void OnUpdate() override;
		virtual void OnEvent(Event& event) override;

	private:
	};

}