#pragma once

#include "Asset.h"
#include "AssetMetadata.h"
#include "AssetManager.h"
#include "EditorAssetManager.h"

#include <filesystem>
#include <unordered_map>

namespace Ludo {

	class AssetImporter
	{
	public:
		static void LoadProjectAssetMetadataList();

		static AssetHandle ImportAsset(const AssetMetadata& metadata);

		static AssetMetadata CreateNewSceneMetadata();
		static AssetMetadata CreateNewTexture2DMetadata();

		static AssetMetadata GetAssetMetadata(const UUID& uuid);

	private:
		static void UpdateProjectAssetMetadataList();

		static inline std::unordered_map<UUID, AssetMetadata> s_MetadataList;
		friend class EditorAssetManager;
	};

}