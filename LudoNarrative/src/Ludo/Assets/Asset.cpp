#include "ldpch.h"
#include "Asset.h"

#include "AssetManager.h"

namespace Ludo {

    AssetHandle::AssetHandle(const UUID& uuid)
        : m_AssetID(uuid)
    {
        if (m_AssetID != 0)
        {
            AssetManager::AddHandleRef(m_AssetID);
        }
    }

    AssetHandle::~AssetHandle()
    {
        if (m_AssetID != 0)
        {
            AssetManager::RemoveHandleRef(m_AssetID);
        }
    }

    AssetHandle AssetHandle::operator=(const AssetHandle& handle)
    {
        UUID id = m_AssetID;
        AssetHandle newhandle(handle.GetUUID());
        AssetManager::RemoveHandleRef(id);

        return newhandle;
    }

    AssetHandle AssetHandle::operator=(const UUID& uuid)
    {
        UUID id = m_AssetID;
        AssetHandle newhandle(uuid);
        AssetManager::RemoveHandleRef(m_AssetID);
        return newhandle;
    }

    AssetType AssetHandle::GetType() const
    {
        return GetAsset<Asset>()->GetAssetType();
    }

    template<typename T>
    inline Ref<T> AssetHandle::GetAsset() const
    {
        Ref<Asset> asset = AssetManager::GetAsset(*this);
        return std::dynamic_pointer_cast<T>(asset);
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