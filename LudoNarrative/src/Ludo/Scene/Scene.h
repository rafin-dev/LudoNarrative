#pragma once

#include "Ludo/Core/TimeStep.h"
#include "Ludo/Renderer/EditorCamera.h"
#include "Ludo/Scene/Components.h"

#include <entt.hpp>

struct b2WorldId;
struct b2BodyId;

namespace Ludo {

	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);
		
		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntitytWithUUID(UUID uuid, const std::string& name);
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateRuntime(TimeStep ts);
		void OnUpdateEditor(TimeStep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity DuplicateEntity(Entity entity);

		void SetMainCamera(Entity camera);
		Entity GetMainCamera();

	private:
		bool m_IsRunning = false;

		entt::entity m_MainCamera;

		// ECS
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		// Box2D
		b2WorldId* m_PhysicsWorld2D = nullptr;

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
		friend class SceneHierarchyPanel;
		friend class ScenePanelHierarchy;
		friend class SceneSerializer;
		friend class Entity;
	};

}