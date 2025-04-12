#pragma once

#include "ViewBase.h"

#include "EntityPropertiesView.h"

namespace Ludo {

	class SceneHierarchyView : public ProjectEditorViewBase
	{
	public:
		SceneHierarchyView(const Ref<EntityPropertiesView>& entityProperties)
			: m_EntityProperties(entityProperties) { }
		~SceneHierarchyView() override;

		void OnImGuiRender() override;

		void SetContext(const Ref<Scene>& context) override { m_Context = context; }
		const Ref<Scene>& GetContext() { return m_Context; }

	private:
		void RenderEntityNode(Entity entity);

		Ref<Scene> m_Context;
		Ref<EntityPropertiesView> m_EntityProperties;
	};

}