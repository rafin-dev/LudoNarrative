#pragma once

#include "ViewBase.h"

namespace Ludo {

	class EntityPropertiesView : public ProjectEditorViewBase
	{
	public:
		~EntityPropertiesView() override;

		void OnImGuiRender() override;
		
		Entity SelectedEntity;
	};

}