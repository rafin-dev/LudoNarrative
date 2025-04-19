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

		std::filesystem::path metadataAbsolutePath = Project::GetMetadataDirectory() / metadata.MetadataFilePath;

		if (!std::filesystem::exists(metadataAbsolutePath.parent_path()))
		{
			std::filesystem::create_directories(metadataAbsolutePath);
		}

		metadata.Serialize(metadata.MetadataFilePath);

		return LoadAsset(metadata.AssetUUID);
	}

	bool EditorAssetManager::IsAssetLoaded(const UUID& uuid)
	{
		return m_LoadedAssets.find(uuid) != m_LoadedAssets.end();
	}

	AssetHandle EditorAssetManager::LoadAsset(const UUID& uuid)
	{
		LD_PROFILE_FUNCTION();

		auto ite = m_LoadedAssets.find(uuid);
		if (ite != m_LoadedAssets.end())
		{
			return ite->second.second->GetHandle();
		}

		AssetMetadata metadata = AssetImporter::GetAssetMetadata(uuid);
		if (metadata.Type == AssetType::None)
		{
			LD_CORE_ERROR("UUID '{0}' is not a valid Asset", (uint64_t)uuid);
			return AssetHandle(0);
		}

		LD_CORE_TRACE("Loading Asset {0}: {1}", AssetTypeToString(metadata.Type).data(), (uint64_t)metadata.AssetUUID);

		Ref<Asset> asset;
		switch (metadata.Type)
		{
			case AssetType::Scene:
			{
				Ref<Scene> scene = CreateRef<Scene>();

				SceneSerializer serializer(scene);
				serializer.DeserializeFromYamlFile(Project::GetAssetDirectory() / metadata.RawFilePath);

				asset = scene;

				break;
			}
			case AssetType::Texture2D:
			{
				Ref<Texture2D> texture = Texture2D::Create(Project::GetAssetDirectory() / metadata.RawFilePath);

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

		if (ite == m_LoadedAssets.end())
		{
			LD_CORE_ERROR("Invalid Asset UUID: {0}", (uint64_t)ite->first);
		}

		ite->second.first++;
	}

	void EditorAssetManager::RemoveHandleRef(const UUID& assetHandle)
	{
		auto ite = m_LoadedAssets.find(assetHandle);

		if (ite == m_LoadedAssets.end())
		{
			LD_CORE_ERROR("Invalid Asset UUID: {0}", (uint64_t)ite->first);
		}

		ite->second.first--;

		if (ite->second.first == 0)
		{
			LD_CORE_TRACE("Unloading Asset {0}: {1}", AssetTypeToString(ite->second.second->GetAssetType()), (uint64_t)ite->first);
			m_LoadedAssets.erase(assetHandle);
		}
	}

}