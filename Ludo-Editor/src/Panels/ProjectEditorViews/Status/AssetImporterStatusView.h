#pragma once

#include "Panels/ProjectEditorViews/ViewBase.h"

#include <Ludo/Assets/AssetImporter.h>

#include <unordered_map>

namespace Ludo {

	class AssetImporterStatusView : public ProjectEditorViewBase
	{
	public:
		AssetImporterStatusView();
		void OnImGuiRender() override;

	private:
		std::unordered_map<AssetType, uint32_t> m_AssetCount;
	};

}