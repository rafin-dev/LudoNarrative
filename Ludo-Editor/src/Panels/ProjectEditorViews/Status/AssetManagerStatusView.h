#pragma once

#include "Panels/ProjectEditorViews/ViewBase.h"

#include <unordered_map>

namespace Ludo {

	class AssetManagerStatusView : public ProjectEditorViewBase
	{
	public:
		AssetManagerStatusView();

		virtual void OnImGuiRender();

	private:
		Ref<EditorAssetManager> m_AssetManager;
		std::unordered_map<AssetType, uint32_t> m_NumberOfAssets;
	};

}