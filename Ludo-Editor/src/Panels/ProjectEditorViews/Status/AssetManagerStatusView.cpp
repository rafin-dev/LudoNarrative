#include "AssetManagerStatusView.h"

namespace Ludo {

	AssetManagerStatusView::AssetManagerStatusView()
	{
		m_NumberOfAssets = 
		{
			{ AssetType::Scene, 0 },
			{ AssetType::Texture2D, 0 }
		};

		m_AssetManager = std::dynamic_pointer_cast<EditorAssetManager>(AssetManager::s_AssetManager);
	}
	
	void AssetManagerStatusView::OnImGuiRender()
	{
		for (auto type : m_NumberOfAssets)
		{
			type.second = 0;

			for (auto& ite : m_AssetManager->m_LoadedAssets)
			{
				if (ite.second.second->GetAssetType() == type.first)
				{
					type.second++;
				}
			}
		}

		ImGui::Begin("Editor Asset Manager Status");

		ImGui::Text("Total Loaded Assets: %i", m_AssetManager->m_LoadedAssets.size());
		ImGui::Text("-- Scene count: %i", m_NumberOfAssets[AssetType::Scene]);
		ImGui::Text("-- Texture2D count: %i", m_NumberOfAssets[AssetType::Scene]);

		ImGui::Separator();

		ImGui::BeginChild("AssetList");

		for (auto& ite : m_AssetManager->m_LoadedAssets)
		{
			ImGui::Text(std::string(AssetTypeToString(ite.second.second->GetAssetType())).c_str());
			ImGui::Text("UUID::%ju", ite.first);
			ImGui::Text("RefCount::%i", (uint32_t)ite.second.first);
		
			ImGui::Separator();
		}

		ImGui::EndChild();

		ImGui::End();
	}

}