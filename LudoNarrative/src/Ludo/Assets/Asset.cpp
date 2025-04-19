#include "ldpch.h"
#include "Asset.h"

#include "AssetManager.h"

namespace Ludo {

    AssetHandle::AssetHandle(const UUID& uuid)
        : m_AssetID(uuid)
    {
        AssetManager::AddHandleRef(m_AssetID);
    }

    AssetHandle::AssetHandle(const AssetHandle& handle)
    {
        m_AssetID = handle.m_AssetID;
        AssetManager::AddHandleRef(m_AssetID);
    }

    AssetHandle::~AssetHandle()
    {
        AssetManager::RemoveHandleRef(m_AssetID);
    }

    AssetHandle& AssetHandle::operator=(const AssetHandle& handle)
    {
        AssetManager::AddHandleRef(handle.GetUUID());
        AssetManager::RemoveHandleRef(m_AssetID);

        m_AssetID = handle.GetUUID();

        return *this;
    }

    AssetHandle& AssetHandle::operator=(const UUID& uuid)
    {
        AssetManager::AddHandleRef(uuid);
        AssetManager::RemoveHandleRef(m_AssetID);

        m_AssetID = uuid;

        return *this;
    }

    AssetType AssetHandle::GetType() const
    {
        return AssetManager::GetAsset<Asset>(m_AssetID)->GetAssetType();
    }

    std::string_view AssetTypeToString(AssetType type)
    {
        switch (type)
        {
        case AssetType::None:      return "AssetType::None";
        case AssetType::Scene:     return "AssetType::Scene";
        case AssetType::Texture2D: return "AssetType::Texture2D";
        }

        return "AssetType::<Invalid>";
    }

    AssetType AssetTypeFromString(std::string_view assetType)
    {
        if (assetType == "AssetType::None")      return AssetType::None;
        if (assetType == "AssetType::Scene")     return AssetType::Scene;
        if (assetType == "AssetType::Texture2D") return AssetType::Texture2D;

        return AssetType::None;
    }

}