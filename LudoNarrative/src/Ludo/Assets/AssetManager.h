#pragma once

#include "Ludo/Core/Core.h"
#include "Ludo/Assets/AssetManagerBase.h"
#include "Ludo/Assets/Asset.h"

namespace Ludo {

	class AssetManager
	{
	public:
		static bool IsAssetLoaded(const UUID& uuid);
		
		static AssetHandle LoadAsset(const UUID& uuid);

		template <typename T>
		static Ref<T> GetAsset(const AssetHandle& handle)
		{
			static_assert(std::is_base_of<Asset, T>::value);

			return std::dynamic_pointer_cast<T>(s_AssetManager->GetAsset(handle));
		}

	private:
		static void AddHandleRef(const UUID& assetHandle);
		static void RemoveHandleRef(const UUID& assetHandle);

		static void InitEditor();
		static void InitRuntime();
		static void Shutdown();

		inline static Ref<AssetManagerBase> s_AssetManager;

		friend class AssetHandle;
		friend class AssetImporter;

		friend class AssetManagerStatusView;

		friend class Application;
		friend class EditorApplication;
	};

}