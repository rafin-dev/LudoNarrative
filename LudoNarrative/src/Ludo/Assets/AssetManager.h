#pragma once

#include "Ludo/Core/Core.h"
#include "Ludo/Assets/AssetManagerBase.h"
#include "Ludo/Assets/Asset.h"

namespace Ludo {

	class AssetManager
	{
	public:
		static AssetHandle LoadAsset(const UUID& uuid);
		static Ref<Asset> GetAsset(const AssetHandle& handle);

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