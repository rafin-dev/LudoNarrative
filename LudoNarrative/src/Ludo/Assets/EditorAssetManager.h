#pragma once

#include "AssetManagerBase.h"

#include "AssetMetadata.h"

#include <atomic>
#include <unordered_map>

namespace Ludo {

	class EditorAssetManager : public AssetManagerBase
	{
	public:
		EditorAssetManager();
		~EditorAssetManager() override;

		AssetHandle ImportAsset(AssetMetadata& metadata);

		AssetHandle LoadAsset(const UUID& uuid) override;
		Ref<Asset> GetAsset(const AssetHandle& handle) override;

		void AddHandleRef(const UUID& assetHandle) override;
		void RemoveHandleRef(const UUID& assetHandle) override;

	private:
		std::unordered_map<UUID, std::pair<std::atomic<uint32_t>, Ref<Asset>>> m_LoadedAssets;
		// Map usage: Asset UUID -> [ AssetHandle Ref Count, Asset ]

		friend class AssetManagerStatusView;
	};

}