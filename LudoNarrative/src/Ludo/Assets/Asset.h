#pragma once

#include "Ludo/Core/UUID.h"

namespace Ludo {

	using AssetHandle = UUID;

	enum class AssetType
	{
		None = 0,
		Scene = 1,
		Texture2D = 2
	};

	std::string_view AssetTypeToString(AssetType type);
	AssetType AssetTypeFromString(std::string_view assetType);

	class Asset
	{
	public:
		const AssetHandle& GetHandle() const { return m_Handle; }

		virtual AssetType GetAssetType() const = 0;

	private:
		AssetHandle m_Handle;
	};

}