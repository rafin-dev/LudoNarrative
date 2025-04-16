#include "ldpch.h"
#include "EditorAssetManager.h"

#include "Ludo/Scene/Scene.h"
#include "Ludo/Scene/SceneSerializer.h"
#include "Ludo/Renderer/Texture.h"
#include "Ludo/Project/Project.h"

#include "AssetImporter.h"
#include "Asset.h"

namespace Ludo {

	EditorAssetManager::EditorAssetManager()
	{
		LD_PROFILE_FUNCTION();

		LD_CORE_TRACE("Initialized Editor Asset Manager");
	}

	EditorAssetManager::~EditorAssetManager()
	{
		LD_PROFILE_FUNCTION();

		LD_CORE_TRACE("Closed Editor Asset Manager");
	}

	AssetHandle EditorAssetManager::ImportAsset(AssetMetadata& metadata)
	{
		LD_PROFILE_FUNCTION();
		
		if (metadata.Type == AssetType::None && std::filesystem::exists(metadata.RawFilePath) )
		{
			LD_CORE_ERROR("Inavalid Asset Metadata");
			return AssetHandle(0);
		}

		std::string relativepath = std::filesystem::relative(metadata.RawFilePath, Project::GetAssetDirectory()).string();
		relativepath.replace(relativepath.find('.'), 1, "-");
		relativepath += ".ldMetadata";

		metadata.MetadataFilePath = Project::GetMetadataDirectory() / relativepath;

		metadata.Serialize(metadata.MetadataFilePath);

		AssetImporter::s_MetadataList.insert(std::pair<UUID, AssetMetadata>(metadata.AssetUUID, metadata));

		return LoadAsset(metadata.AssetUUID);
	}

	AssetHandle EditorAssetManager::LoadAsset(const UUID& uuid)
	{
		LD_PROFILE_FUNCTION();

		AssetMetadata metadata = AssetImporter::GetAssetMetadata(uuid);
		if (metadata.Type == AssetType::None)
		{
			LD_CORE_ERROR("UUID '{0}' is not a valid Asset", (uint64_t)uuid);
			return AssetHandle(0);
		}

		Ref<Asset> asset;
		switch (metadata.Type)
		{
			case AssetType::Scene:
			{
				Ref<Scene> scene = CreateRef<Scene>();

				SceneSerializer serializer(scene);
				serializer.DeserializeFromYamlFile(metadata.RawFilePath);

				asset = scene;

				break;
			}
			case AssetType::Texture2D:
			{
				Ref<Texture2D> texture = Texture2D::Create(metadata.RawFilePath);

				asset = texture;

				break;
			}
		}

		asset->m_Handle = metadata.AssetUUID; // make the loaded asset have the correct UUID

		m_LoadedAssets.insert(std::pair(metadata.AssetUUID, std::pair(0, asset)));
		return asset->GetHandle();
	}

	Ref<Asset> EditorAssetManager::GetAsset(const AssetHandle& asset)
	{
		auto ite = m_LoadedAssets.find(asset.GetUUID());
		LD_CORE_ASSERT(ite != m_LoadedAssets.end(), "Invalid Asset Handle");

		return ite->second.second;
	}

	void EditorAssetManager::AddHandleRef(const UUID& assetHandle)
	{
		auto ite = m_LoadedAssets.find(assetHandle);
		LD_CORE_ASSERT(ite != m_LoadedAssets.end(), "Creation of invalid AssetHandle");

		ite->second.first++;
	}

	void EditorAssetManager::RemoveHandleRef(const UUID& assetHandle)
	{
		auto ite = m_LoadedAssets.find(assetHandle);
		LD_CORE_ASSERT(ite != m_LoadedAssets.end(), "Invalid Asset Handle");

		ite->second.first--;

		if (ite->second.first == 0)
		{
			m_LoadedAssets.erase(assetHandle);
		}
	}

}