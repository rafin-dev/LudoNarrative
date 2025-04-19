#pragma once

#include <LudoNarrative.h>

namespace Ludo {

	class ProjectEditorViewBase
	{
	public:
		virtual ~ProjectEditorViewBase() = default;

		virtual void OnUpdate(TimeStep ts) {}
		virtual void OnImGuiRender() {}

		virtual bool OnEvent(Event& event) { return false; }

		bool Active = true;
	};

}