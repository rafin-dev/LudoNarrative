#pragma once

#include "Asset.h"

#include <chrono>
#include <filesystem>

namespace Ludo {

	struct SceneMetadata
	{
		uint32_t StartEntityCount;
	};

	struct Texture2DMetadata
	{
		uint32_t Width;
		uint32_t Height;

		std::filesystem::path PixleDataFilePath; // File path to ready to use pixel data
	};

	struct AssetMetadata
	{
		AssetType Type = AssetType::None;
		UUID AssetUUID; // UUID innstead of AssetHandle as to not interfere with ref counting
		
		// Relative to the asset directory
		std::filesystem::path RawFilePath; // Path to the asset's original file (.png, .obg, .wav ...)
		std::chrono::time_point<std::chrono::system_clock> RawFileLastUpdated; // If this != std::filesystem::last_write_time() reimport asset
		
		// Relatie to the Metadata directory
		std::filesystem::path MetadataFilePath;

		// Only the type matching metadata struct will be filled
		SceneMetadata SceneData;
		Texture2DMetadata Texture2DData;

		void Serialize(const std::filesystem::path& path);
		void Deserialize(const std::filesystem::path& path);
	};

}