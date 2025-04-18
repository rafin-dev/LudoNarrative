#pragma once

#include "Ludo/Core/TimeStep.h"
#include "Ludo/Renderer/EditorCamera.h"
#include "Ludo/Scene/Components.h"
#include "Ludo/Assets/Asset.h"

#include <entt.hpp>

struct b2WorldId;

namespace Ludo {

	class Entity;

	class Scene : public Asset
	{
	public:
		Scene();
		Scene(const std::string& name);
		~Scene();

		AssetType GetAssetType() const override { return AssetType::Scene; }

		static Ref<Scene> Copy(Ref<Scene> other);
		
		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntitytWithUUID(UUID uuid, const std::string& name);
		void DestroyEntity(Entity entity);

		Entity GetEntityByUUID(const UUID& uuid);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulateStart();
		void OnSimulateStop();

		void OnUpdateRuntime(TimeStep ts);
		void OnUpdateEditor(TimeStep ts, EditorCamera& camera);
		void OnUpdateSimulate(TimeStep ts, EditorCamera& camera);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity DuplicateEntity(Entity entity);

		void SetMainCamera(Entity camera);
		Entity GetMainCamera();

		const std::string& GetName() { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:
		void StartPhysics2D();
		void StopPhysics2D();

		bool m_IsRunning = false;

		entt::entity m_MainCamera;

		// ECS
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		std::unordered_map<UUID, Entity> m_EntityByUUID;

		// Box2D
		b2WorldId* m_PhysicsWorld2D = nullptr;

		std::string m_Name;

		// OnComponentAdd and it's specializations
		template<typename T>
		void OnComponentAdded(Entity entity, T& component) {}
		template<>
		void OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component);
		template<>
		void OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component);
		template<>
		void OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component);

		// Friends
		friend class SceneHierarchyView;
		friend class SceneSerializer;
		friend class Entity;
	};

}