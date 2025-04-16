#pragma once

#include "Panels/ProjectEditorViews/ViewBase.h"

namespace Ludo {

	class AssetManagerStatusView : public ProjectEditorViewBase
	{
	public:
		AssetManagerStatusView();

		virtual void OnImGuiRender();

	private:
		Ref<EditorAssetManager> m_AssetManager;
	};

}