#pragma once

#include "Ludo/Assets/Asset.h"

namespace Ludo {

	class AssetManagerBase
	{
	public:
		virtual ~AssetManagerBase() = default;

		virtual AssetHandle LoadAsset(const UUID& uuid) = 0;
		virtual Ref<Asset> GetAsset(const AssetHandle& handle) = 0;

		virtual void AddHandleRef(const UUID& assetHandle) = 0;
		virtual void RemoveHandleRef(const UUID& assetHandle) = 0;

	};

}