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

		virtual void SetContext(const Ref<Scene>& context) {};
		virtual const Ref<Scene>& GetContext() { return nullptr; };

		bool Active = true;
	};

}