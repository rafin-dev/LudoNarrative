#pragma once

#include "Ludo/Scene/Scene.h"
#include "Ludo/Scene/Components.h"

#include <entt.hpp>

namespace Ludo {

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity entityHandle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			LD_CORE_ASSERT(!HasComponent<T>(), "Entity alredy has component");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			LD_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			LD_CORE_ASSERT(HasComponent<T>(), "Entity does not have component");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		const std::string& GetName()
		{
			LD_CORE_ASSERT(HasComponent<TagComponent>(), "Entity does not have TagComponent");
			return GetComponent<TagComponent>().Tag;
		}

		UUID GetUUID()
		{
			LD_CORE_ASSERT(HasComponent<IDComponent>(), "Entity does not Have IDComponent");
			return GetComponent<IDComponent>().ID;
		}

		void SetAsMainCamera()
		{
			m_Scene->SetMainCamera(*this);
		}

		bool IsMainCamera()
		{
			return m_Scene->GetMainCamera() == *this;
		}

		operator bool() const { return m_EntityHandle != entt::null && m_Scene->m_Registry.valid(m_EntityHandle); }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }

		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }

	private:
		entt::entity m_EntityHandle = entt::null;
		Scene* m_Scene = nullptr;
	};

}