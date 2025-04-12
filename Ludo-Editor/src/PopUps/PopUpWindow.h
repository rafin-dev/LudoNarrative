#pragma once

#include "LudoNarrative.h"

namespace Ludo {

	class PopUpWindow
	{
	public:
		virtual ~PopUpWindow() = default;

		virtual void OnUpdate(TimeStep ts) {}
		virtual void OnImGuiRender() {}

		virtual bool OnEvent(Event& event) { return false; }

		virtual bool ShouldClose() = 0;
	};

}