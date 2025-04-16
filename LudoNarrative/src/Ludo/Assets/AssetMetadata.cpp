#include "ldpch.h"
#include "AssetMetadata.h"

#include <filesystem>

#include <yaml-cpp/yaml.h>

namespace Ludo {

	void AssetMetadata::Serialize(const std::filesystem::path& path)
	{
		MetadataFilePath = path;

		YAML::Emitter out;

		out << YAML::BeginMap;

		out << YAML::Key << "AssetType" << YAML::Value << AssetTypeToString(Type);
		out << YAML::Key << "AssetUUID" << YAML::Value << AssetUUID;
		out << YAML::Key << "RawFilePath" << YAML::Value << RawFilePath.string();
		out << YAML::Key << "RawFileLastUpdated" << YAML::Value << std::chrono::system_clock::to_time_t(RawFileLastUpdated);
		out << YAML::Key << "MetadataFilePath" << YAML::Value << MetadataFilePath.string();

		switch (Type)
		{
		case Ludo::AssetType::Scene:
			out << YAML::Key << "SceneMetadata" << YAML::Value << YAML::BeginMap;

			out << YAML::Key << "StartEntityCount" << YAML::Value << SceneData.StartEntityCount;

			out << YAML::EndMap; // Scene
			break;

		case Ludo::AssetType::Texture2D:
			out << YAML::Key << "Texture2DMetadata" << YAML::Value << YAML::BeginMap;

			out << YAML::Key << "Width" << YAML::Value << Texture2DData.Width;
			out << YAML::Key << "Height" << YAML::Value << Texture2DData.Height;
			out << YAML::Key << "PixelDataFilePath" << YAML::Value << Texture2DData.PixleDataFilePath.string();

			out << YAML::EndMap; // Tetxure2D
			break;
		}

		out << YAML::EndMap;

		std::ofstream stream(path);
		stream << out.c_str();
	}

	void AssetMetadata::Deserialize(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			LD_CORE_ERROR("Failed to load Asset Metadata at '{0}': File does not exist", path.string());
		}

		std::ifstream stream(path);
		std::stringstream sstream;
		sstream << stream.rdbuf();

		YAML::Node data = YAML::Load(sstream);

	}

}