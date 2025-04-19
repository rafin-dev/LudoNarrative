#pragma once

#include "LudoNarrative.h"

#include <list>

namespace Ludo {

	class OpenAndSelectedsManager
	{
	public:
		AssetHandle GetFocusedScene() { return m_FocusedScene; }
		Entity GetFocusedEntity() { return m_FocusedEntity; }

		void AddScene(const AssetHandle& scene);
		void RemoveScene(const AssetHandle& scene);

		void SetFocusedScene(const AssetHandle& scene);
		void SetFocusedEntity(Entity entity);

		const std::list<AssetHandle>& GetSceneList() { return m_LoadedScenes; }

	private:
		AssetHandle m_FocusedScene;
		Entity m_FocusedEntity;
		std::list<AssetHandle> m_LoadedScenes;
	};

}