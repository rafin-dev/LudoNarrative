#include "ldpch.h"
#include "AssetImporter.h"

#include "Ludo/Project/Project.h"

#include <yaml-cpp/yaml.h>

namespace Ludo {
    
#define METADATA_LIST_FILENAME "AssetMetadataList.ldMetadata"

    void AssetImporter::LoadProjectAssetMetadataList()
    {
        LD_PROFILE_FUNCTION();

        std::filesystem::path metadataListPath = Project::GetMetadataDirectory() / METADATA_LIST_FILENAME;

        if (!std::filesystem::exists(metadataListPath)) // Metadata list has yet to be created, nothing to load
        {
            return;
        }

        std::ifstream stream(metadataListPath);
        std::stringstream sstream;
        sstream << stream.rdbuf();

        YAML::Node data = YAML::Load(sstream);

        if (!data["MetadataFileList"])
        {
            LD_CORE_ERROR("Failed to load project's Asset metadata: MetadataFileList missing from {0}", metadataListPath.string());
            return;
        }

        s_MetadataList.clear();

        for (auto metadataData : data["MetadataFileList"])
        {
            AssetMetadata metadata;
            metadata.Deserialize(metadataData.as<std::string>());

            s_FileToUUID.insert(std::pair(metadata.RawFilePath, metadata.AssetUUID));
            s_MetadataList.insert(std::pair(metadata.AssetUUID, metadata));
        }
    }
    
    AssetHandle AssetImporter::ImportAsset(const AssetMetadata& metadata)
    {
        LD_PROFILE_FUNCTION();

        Ref<EditorAssetManager> assetManager = std::dynamic_pointer_cast<EditorAssetManager>(AssetManager::s_AssetManager);

        LD_CORE_ASSERT(assetManager, "Atempt to import Asset with runtime AssetManager");

        AssetMetadata mtd = metadata;

        AddMetadata(mtd);

        AssetHandle handle = assetManager->ImportAsset(mtd);

        return handle;
    }

    AssetMetadata AssetImporter::CreateNewSceneMetadata()
    {
        AssetMetadata metadata;
        metadata.Type = AssetType::Scene;

        return metadata;
    }

    AssetMetadata AssetImporter::CreateNewTexture2DMetadata()
    {
        AssetMetadata metadata;
        metadata.Type = AssetType::Texture2D;

        return metadata;
    }

    AssetMetadata AssetImporter::GetAssetMetadata(const std::filesystem::path& rawFilePath)
    {
        auto relative = std::filesystem::relative(rawFilePath, Project::GetAssetDirectory());
        auto ite = s_FileToUUID.find(relative);
        if (ite == s_FileToUUID.end())
        {
            return AssetMetadata();
        }

        return s_MetadataList[ite->second];
    }

    AssetMetadata AssetImporter::GetAssetMetadata(const UUID& uuid)
    {
        LD_PROFILE_FUNCTION();
    
        auto ite = s_MetadataList.find(uuid);

        if (ite != s_MetadataList.end())
        {
            return ite->second;
        }

        return AssetMetadata();
    }

    void AssetImporter::AddMetadata(AssetMetadata& metadata)
    {
        std::string relativepath = metadata.RawFilePath.string();
        relativepath.replace(relativepath.find('.'), 1, "-");
        relativepath += ".ldMetadata";

        metadata.MetadataFilePath = relativepath;

        s_FileToUUID.insert(std::pair(metadata.RawFilePath, metadata.AssetUUID));
        s_MetadataList.insert(std::pair(metadata.AssetUUID, metadata));

        UpdateProjectAssetMetadataList();
    }

    void AssetImporter::UpdateProjectAssetMetadataList()
    {
        LD_PROFILE_FUNCTION();

        YAML::Emitter out;

        out << YAML::BeginMap;

        out << YAML::Key << "MetadataFileList" << YAML::Value << YAML::BeginSeq;

        for (auto& metadata : s_MetadataList)
        {
            out << metadata.second.MetadataFilePath.string();
        }

        out << YAML::EndSeq;

        out << YAML::EndMap;

        std::filesystem::path path = Project::GetMetadataDirectory() / METADATA_LIST_FILENAME;

        std::ofstream stream(path);
        stream << out.c_str();
    }

}