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

		static AssetMetadata GetAssetMetadata(const std::filesystem::path& rawFilePath); // Path relative to Asset Folder
		static AssetMetadata GetAssetMetadata(const UUID& uuid);

	private:
		static void AddMetadata(AssetMetadata& metadata);
		static void UpdateProjectAssetMetadataList();

		static inline std::unordered_map<std::filesystem::path, UUID> s_FileToUUID;
		static inline std::unordered_map<UUID, AssetMetadata> s_MetadataList;

		friend class AssetImporterStatusView;
		friend class TexturePickerPopUp;
	};

}