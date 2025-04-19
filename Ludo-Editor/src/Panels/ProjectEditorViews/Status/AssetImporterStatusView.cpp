#include "AssetImporterStatusView.h"

namespace Ludo {

	AssetImporterStatusView::AssetImporterStatusView()
	{
		m_AssetCount = {
			{ AssetType::Scene, 0 },
			{ AssetType::Texture2D, 0}
		};
	}

	void AssetImporterStatusView::OnImGuiRender()
	{
		for (auto& type : m_AssetCount)
		{
			type.second = 0;
			for (auto& asset : AssetImporter::s_MetadataList)
			{
				if (type.first == asset.second.Type)
				{
					type.second++;
				}
			}
		}

		ImGui::Begin("Asset Importer Status");

		ImGui::Text("Total project Asset Number: %i", AssetImporter::s_MetadataList.size());
		ImGui::Text("-- Scene Count:     %i", m_AssetCount[AssetType::Scene]);
		ImGui::Text("-- Texture2D Count: %i", m_AssetCount[AssetType::Texture2D]);

		ImGui::Separator();

		ImGui::BeginChild("AssetList");

		for (auto& asset : AssetImporter::s_MetadataList)
		{
			ImGui::Text(std::string(AssetTypeToString(asset.second.Type)).c_str());
			ImGui::Text("UUID::%ju", asset.first);
			ImGui::Text("Loaded::%s", (AssetManager::IsAssetLoaded(asset.first) ? "True" : "False"));
			
			ImGui::Separator();
		}

		ImGui::EndChild();

		ImGui::End();
	}

}