#include "ldpch.h"
#include "AssetMetadata.h"

#include "Ludo/Project/Project.h"

#include <filesystem>

#include <yaml-cpp/yaml.h>

namespace Ludo {

	void AssetMetadata::Serialize(const std::filesystem::path& path)
	{
		std::filesystem::path absolute = Project::GetMetadataDirectory() / path;

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
			out << YAML::Key << "PixelDataFilePath" << YAML::Value << std::filesystem::relative(Texture2DData.PixleDataFilePath, Project::GetMetadataDirectory()).string();

			out << YAML::EndMap; // Tetxure2D
			break;
		}

		out << YAML::EndMap;

		std::ofstream stream(absolute);
		stream << out.c_str();
	}

	void AssetMetadata::Deserialize(const std::filesystem::path& path)
	{
		std::filesystem::path absolute = Project::GetMetadataDirectory() / path;

		if (!std::filesystem::exists(absolute))
		{
			LD_CORE_ERROR("Failed to load Asset Metadata at '{0}': File does not exist", absolute.string());
		}

		std::ifstream stream(absolute);
		std::stringstream sstream;
		sstream << stream.rdbuf();

		YAML::Node data = YAML::Load(sstream);

		Type = AssetTypeFromString(data["AssetType"].as<std::string>());
		AssetUUID = data["AssetUUID"].as<uint64_t>();
		RawFilePath = data["RawFilePath"].as<std::string>();
		RawFileLastUpdated = std::chrono::system_clock::from_time_t(data["RawFileLastUpdated"].as<time_t>());
		MetadataFilePath = data["MetadataFilePath"].as<std::string>();

		switch (Type)
		{
		case Ludo::AssetType::Scene:
			SceneData.StartEntityCount = data["SceneMetadata"]["StartEntityCount"].as<uint32_t>();
			break;

		case Ludo::AssetType::Texture2D:
			Texture2DData.Width = data["Texture2DMetadata"]["Width"].as<uint32_t>();
			Texture2DData.Height = data["Texture2DMetadata"]["Height"].as<uint32_t>();
			Texture2DData.PixleDataFilePath = data["Texture2DMetadata"]["PixelDataFilePath"].as<std::string>();

			break;
		}
	}

}