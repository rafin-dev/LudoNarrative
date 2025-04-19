#pragma once

#include "ViewBase.h"

#include "misc/OpenAndSelectedsManager.h"

namespace Ludo {

	class SceneHierarchyView : public ProjectEditorViewBase
	{
	public:
		SceneHierarchyView(const Ref<OpenAndSelectedsManager>& oas)
			: m_OpenAndSelecteds(oas) { }
		~SceneHierarchyView() override;

		void OnImGuiRender() override;

	private:
		void RenderEntityNode(Entity entity);

		Ref<OpenAndSelectedsManager> m_OpenAndSelecteds;
	};

}