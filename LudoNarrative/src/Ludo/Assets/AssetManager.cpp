#include "ldpch.h"
#include "AssetManager.h"

#include "EditorAssetManager.h"
#include "RuntimeAssetManager.h"

namespace Ludo {

    bool AssetManager::IsAssetLoaded(const UUID& uuid)
    {
        return s_AssetManager->IsAssetLoaded(uuid);
    }
    
    AssetHandle AssetManager::LoadAsset(const UUID& uuid)
    {
        return s_AssetManager->LoadAsset(uuid);
    }

    void AssetManager::AddHandleRef(const UUID& assetHandle)
    {
        if (assetHandle == 0)
        {
            return;
        }

        s_AssetManager->AddHandleRef(assetHandle);
    }

    void AssetManager::RemoveHandleRef(const UUID& assetHandle)
    {
        if (assetHandle == 0)
        {
            return;
        }

        s_AssetManager->RemoveHandleRef(assetHandle);
    }

    void AssetManager::InitEditor()
    {
        s_AssetManager = CreateRef<EditorAssetManager>();
    }

    void AssetManager::InitRuntime()
    {
        s_AssetManager = CreateRef<RuntimeAssetManager>();
    }

    void AssetManager::Shutdown()
    {
        s_AssetManager = nullptr;
    }

}