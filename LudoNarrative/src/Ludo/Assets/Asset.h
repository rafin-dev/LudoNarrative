#pragma once

#include "Ludo/Core/UUID.h"

namespace Ludo {

	enum class AssetType
	{
		None = 0,
		Scene = 1,
		Texture2D = 2
	};

	class AssetHandle
	{
	public:
		AssetHandle() { m_AssetID = 0; }
		AssetHandle(const AssetHandle& handle);
		~AssetHandle();

		AssetHandle& operator=(const AssetHandle& handle);
		AssetHandle& operator=(const UUID& uuid);

		AssetType GetType() const;

		const UUID& GetUUID() const { return m_AssetID; }
		bool IsValid() const { return m_AssetID != 0; }

		bool operator==(const AssetHandle& other) const
		{
			return m_AssetID == other.m_AssetID;
		}

		operator bool()
		{
			return IsValid();
		}

	private:
		AssetHandle(const UUID& uuid);
		UUID m_AssetID;

		friend class Asset;
		friend class EditorAssetManager;
		friend class RuntimeAssetManager;
	};

	std::string_view AssetTypeToString(AssetType type);
	AssetType AssetTypeFromString(std::string_view assetType);

	class Asset
	{
	public:
		AssetHandle GetHandle() const { return AssetHandle(m_Handle); }

		virtual AssetType GetAssetType() const = 0;

	private:
		UUID m_Handle;

		friend class RuntimeAssetManager;
		friend class EditorAssetManager;
	};

}