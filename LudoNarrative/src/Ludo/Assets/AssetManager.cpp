#include "ldpch.h"
#include "AssetManager.h"

#include "EditorAssetManager.h"
#include "RuntimeAssetManager.h"

namespace Ludo {

    AssetHandle AssetManager::LoadAsset(const UUID& uuid)
    {
        return s_AssetManager->LoadAsset(uuid);
    }

    Ref<Asset> AssetManager::GetAsset(const AssetHandle& handle)
    {
        return s_AssetManager->GetAsset(handle);
    }

    void AssetManager::AddHandleRef(const UUID& assetHandle)
    {
        s_AssetManager->AddHandleRef(assetHandle);
    }

    void AssetManager::RemoveHandleRef(const UUID& assetHandle)
    {
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