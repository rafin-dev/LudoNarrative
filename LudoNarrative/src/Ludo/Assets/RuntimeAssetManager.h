#pragma once

#include "AssetManagerBase.h"

namespace Ludo {

	class RuntimeAssetManager : public AssetManagerBase
	{
	public:
		~RuntimeAssetManager() override {}

		bool IsAssetLoaded(const UUID& uuid) override { return false; }
		AssetHandle	LoadAsset(const UUID& uuid) override { return AssetHandle(0); }
		Ref<Asset> GetAsset(const AssetHandle& handle) override { return nullptr; }

		void AddHandleRef(const UUID& assetHandle) override {}
		void RemoveHandleRef(const UUID& assetHandle) override {}
	};

}