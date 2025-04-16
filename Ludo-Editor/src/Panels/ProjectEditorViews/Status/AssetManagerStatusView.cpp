#include "AssetManagerStatusView.h"

namespace Ludo {

	AssetManagerStatusView::AssetManagerStatusView()
	{
		m_AssetManager = std::dynamic_pointer_cast<EditorAssetManager>(AssetManager::s_AssetManager);
	}
	
	void AssetManagerStatusView::OnImGuiRender()
	{
		ImGui::Begin("Editor Asset Manager Status");



		ImGui::End();
	}

}