#include "OpenAndSelectedsManager.h"

namespace Ludo {

	void OpenAndSelectedsManager::AddScene(const AssetHandle& scene)
	{
		if (std::find(m_LoadedScenes.begin(), m_LoadedScenes.end(), scene) != m_LoadedScenes.end())
		{
			return; // Scene alredy in
		}

		m_LoadedScenes.emplace_back(scene);
	}

	void OpenAndSelectedsManager::RemoveScene(const AssetHandle& scene)
	{
		if (scene == m_FocusedScene)
		{
			m_FocusedScene = AssetHandle();
		}

		m_LoadedScenes.remove(scene);
	}

	void OpenAndSelectedsManager::SetFocusedScene(const AssetHandle& scene)
	{
		if (std::find(m_LoadedScenes.begin(), m_LoadedScenes.end(), scene) == m_LoadedScenes.end())
		{
			m_LoadedScenes.emplace_back(scene);
		}

		if (m_FocusedScene != scene)
		{
			m_FocusedEntity = Entity();
		}

		m_FocusedScene = scene;
	}

	void OpenAndSelectedsManager::SetFocusedEntity(Entity entity)
	{
		m_FocusedEntity = entity;
	}

}