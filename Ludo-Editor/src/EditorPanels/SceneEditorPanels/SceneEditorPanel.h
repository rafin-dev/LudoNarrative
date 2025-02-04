#pragma once

#include <LudoNarrative.h>

namespace Ludo {

	class SceneEditorPanel
	{
	public:
		virtual ~SceneEditorPanel() = default;

		virtual void OnImGuiRender() = 0;

		virtual void SetContext(const Ref<Scene>& context) = 0;
		virtual const Ref<Scene>& GetContext() const = 0;

		virtual bool IsActive() const = 0;
		virtual void SetActive(bool active) = 0;
	};

}