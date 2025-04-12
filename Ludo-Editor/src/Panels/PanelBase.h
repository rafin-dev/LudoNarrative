#pragma once

#include <LudoNarrative.h>

namespace Ludo {

	enum class PanelCodes
	{
		ProjectSelector = 0,
		ProjectEditor = 1
	};

	class PanelBase
	{
	public:
		virtual ~PanelBase() = default;

		// Called when the panel becomes the main panel
		virtual void OnAttach() {}
		// Called when the panel loses main panel status
		virtual void OnDetach() {}

		virtual void OnUpdate(TimeStep ts) {}
		virtual void OnImGui() {}

		virtual void OnEvent(Event& e) {}
	
		// Return true to allow the panel change, false to block
		virtual bool OnPanelChange(PanelCodes newPanel) { return true; }
	};

}