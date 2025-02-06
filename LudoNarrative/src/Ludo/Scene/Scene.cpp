#include "ldpch.h"
#include "Scene.h"

#include "Ludo/Renderer/Renderer2D.h"
#include "Ludo/Scene/Entity.h"
#include "Ludo/Scene/ScriptableEntity.h"

#include <DirectXMath.h>

#include <box2d/box2d.h>
#include <box2d/collision.h>

namespace Ludo {

	// Internal components not exposed to the editor, used only to store Box2D IDs
	struct Rigidbody2DStorageComponent
	{
		b2BodyId BodyID = b2_nullBodyId;
	};


	Scene::Scene()
	{
	}

	Scene::Scene(const std::string& name)
		: m_Name(name)
	{
	}

	Scene::~Scene()
	{
		if (m_PhysicsWorld2D != nullptr)
		{
			b2DestroyWorld(*m_PhysicsWorld2D);
		}
	}

	template<typename Component>
	static void CopyComponents(entt::registry& dest, const entt::registry& src, const std::unordered_map<UUID, entt::entity> enttMap)
	{
		auto view = src.view<Component>();
		for (auto srcE : view)
		{
			auto& component = view.get<Component>(srcE);
			auto& uuid = src.get<IDComponent>(srcE).ID;

			LD_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());
			dest.emplace_or_replace<Component>(enttMap.at(uuid), component);
		}
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		Entity mainCamera = other->GetMainCamera();
		std::unordered_map<UUID, entt::entity> enttMap;
		auto view = other->m_Registry.view<entt::entity>();
		for (auto ite = view.rbegin(); ite != view.rend(); ite++) // reverse iterate to preserve the entity order
		{
			Entity entity(*ite, other.get());

			UUID uuid = entity.GetUUID();
			auto& name = entity.GetName();
			Entity newEntity = newScene->CreateEntitytWithUUID(uuid, name);
			if (mainCamera && uuid == mainCamera.GetUUID())
			{
				newScene->m_MainCamera = Entity(newEntity, newScene.get());
			}

			enttMap.insert(std::pair(uuid, *ite));
		}

		CopyComponents<TransformComponent>(newScene->m_Registry, other->m_Registry, enttMap);
		CopyComponents<SpriteRendererComponent>(newScene->m_Registry, other->m_Registry, enttMap);
		CopyComponents<CircleRendererComponent>(newScene->m_Registry, other->m_Registry, enttMap);
		CopyComponents<CameraComponent>(newScene->m_Registry, other->m_Registry, enttMap);
		CopyComponents<NativeScriptComponent>(newScene->m_Registry, other->m_Registry, enttMap);
		CopyComponents<Rigidbody2DComponent>(newScene->m_Registry, other->m_Registry, enttMap);
		CopyComponents<BoxCollider2DComponent>(newScene->m_Registry, other->m_Registry, enttMap);
		CopyComponents<CircleCollider2DComponent>(newScene->m_Registry, other->m_Registry, enttMap);

		// Internal Components
		CopyComponents<Rigidbody2DStorageComponent>(newScene->m_Registry, other->m_Registry, enttMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntitytWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntitytWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity(m_Registry.create(), this);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);
		entity.AddComponent<IDComponent>(uuid);

		m_EntityByUUID.insert(std::pair(uuid, entity));

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		if (entity == m_MainCamera)
		{
			m_MainCamera = Entity();
		}

		m_Registry.destroy(entity);
	}

	Entity Scene::GetEntityByUUID(const UUID& uuid)
	{
		auto ite = m_EntityByUUID.find(uuid);

		return ite != m_EntityByUUID.end() ? ite->second : Entity();
	}

	static b2BodyType GetBox2DBodyType(Rigidbody2DComponent::BodyType type)
	{
		switch (type)
		{
		case Rigidbody2DComponent::BodyType::Static:
			return b2BodyType::b2_staticBody;
		case Rigidbody2DComponent::BodyType::Dynamic:
			return b2BodyType::b2_dynamicBody;
		case Rigidbody2DComponent::BodyType::Kinematic:
			return b2BodyType::b2_kinematicBody;
		}

		LD_CORE_ASSERT(false, "Unknown Body type");
		return b2BodyType::b2_staticBody;
	}

	void Scene::OnRuntimeStart()
	{
		m_IsRunning = true;

		// Initialize scripts
		{
			auto view = m_Registry.view<NativeScriptComponent>();
			for (auto entityID : view)
			{
				auto& nativeScript = view.get<NativeScriptComponent>(entityID);

				nativeScript.Instance = nativeScript.InstantiateScript();
				nativeScript.Instance->m_Entity = Entity(entityID, this);
				nativeScript.Instance->OnCreate();
			}
		}

		StartPhysics2D();
	}

	void Scene::OnRuntimeStop()
	{
		m_IsRunning = false;

		// Unload Scripts
		auto view = m_Registry.view<NativeScriptComponent>();
		for (auto entityID : view)
		{
			auto& nativeScript = view.get<NativeScriptComponent>(entityID);

			nativeScript.Instance->OnDestroy();
			nativeScript.DestroyScript(&nativeScript);
		}

		StopPhysics2D();
	}

	void Scene::OnSimulateStart()
	{
		StartPhysics2D();
	}

	void Scene::OnSimulateStop()
	{
		StopPhysics2D();
	}

	void Scene::OnUpdateRuntime(TimeStep ts)
	{
		// Update Scripts
		{
			auto group = m_Registry.group<NativeScriptComponent>();
			for (auto entityID : group)
			{
				auto& nsc = group.get<NativeScriptComponent>(entityID);

				nsc.Instance->OnUpdate(ts);
			}
		}

		// Physics 2D
		{
			b2World_Step(*m_PhysicsWorld2D, ts, 4);

			auto group = m_Registry.group<Rigidbody2DStorageComponent>(entt::get<TransformComponent>);
			for (auto entityID : group)
			{
				auto [storage, transform] = group.get<Rigidbody2DStorageComponent, TransformComponent>(entityID);

				auto pos = b2Body_GetPosition(storage.BodyID);
				auto rot = b2Body_GetRotation(storage.BodyID);

				transform.Translation.x = pos.x;
				transform.Translation.y = pos.y;
				transform.Rotation.z = std::atan2(rot.s, rot.c);
			}
		}

		Entity mainCamera(m_MainCamera, this);
		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera.GetComponent<CameraComponent>().Camera, mainCamera.GetComponent<TransformComponent>().GetTransform());

			// Sprites
			{
				auto group = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
				for (auto entityID : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entityID);
					DirectX::XMMATRIX spriteTransform;
					transform.GetTransform(&spriteTransform);

					Renderer2D::DrawSprite(spriteTransform, sprite, (int)entityID);
				}
			}

			// Circles
			{
				auto group = m_Registry.group<CircleRendererComponent>(entt::get<TransformComponent>);
				for (auto entityID : group)
				{
					auto [transform, circle] = group.get<TransformComponent, CircleRendererComponent>(entityID);
					DirectX::XMMATRIX circleTransform;
					transform.GetTransform(&circleTransform);

					Renderer2D::DrawCircle(circleTransform, circle, (int)entityID);
				}
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdateEditor(TimeStep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		{
			// Sprites
			auto group = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
			for (auto entityID : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entityID);
				DirectX::XMMATRIX spriteTransform;
				transform.GetTransform(&spriteTransform);

				Renderer2D::DrawSprite(spriteTransform, sprite, (int)entityID);
			}
		}

		{
			// Circles
			auto group = m_Registry.group<CircleRendererComponent>(entt::get<TransformComponent>);
			for (auto entityID : group)
			{
				auto [transform, circle] = group.get<TransformComponent, CircleRendererComponent>(entityID);
				DirectX::XMMATRIX circleTransform;
				transform.GetTransform(&circleTransform);

				Renderer2D::DrawCircle(circleTransform, circle, (int)entityID);
			}
		}

		Renderer2D::EndScene();
	}

	void Scene::OnUpdateSimulate(TimeStep ts, EditorCamera& camera)
	{
		// Physics 2D
		{
			b2World_Step(*m_PhysicsWorld2D, ts, 4);

			auto group = m_Registry.group<Rigidbody2DStorageComponent>(entt::get<TransformComponent>);
			for (auto entityID : group)
			{
				auto [storage, transform] = group.get<Rigidbody2DStorageComponent, TransformComponent>(entityID);

				auto pos = b2Body_GetPosition(storage.BodyID);
				auto rot = b2Body_GetRotation(storage.BodyID);

				transform.Translation.x = pos.x;
				transform.Translation.y = pos.y;
				transform.Rotation.z = std::atan2(rot.s, rot.c);
			}
		}

		Renderer2D::BeginScene(camera);
		{
			// Sprites
			auto group = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
			for (auto entityID : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entityID);
				DirectX::XMMATRIX spriteTransform;
				transform.GetTransform(&spriteTransform);

				Renderer2D::DrawSprite(spriteTransform, sprite, (int)entityID);
			}
		}

		{
			// Circles
			auto group = m_Registry.group<CircleRendererComponent>(entt::get<TransformComponent>);
			for (auto entityID : group)
			{
				auto [transform, circle] = group.get<TransformComponent, CircleRendererComponent>(entityID);
				DirectX::XMMATRIX circleTransform;
				transform.GetTransform(&circleTransform);

				Renderer2D::DrawCircle(circleTransform, circle, (int)entityID);
			}
		}
		Renderer2D::EndScene();
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}

	}

	template<typename Component>
	static void CopyComponentIfExists(Entity dest, Entity src)
	{
		if (src.HasComponent<Component>())
		{
			dest.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		Entity newEntity = CreateEntity(entity.GetName());

		CopyComponentIfExists<TransformComponent>(newEntity, entity);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, entity);
		CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<CircleCollider2DComponent>(newEntity, entity);

		return Entity();
	}

	void Scene::SetMainCamera(Entity camera)
	{
		if (camera)
		{
			LD_CORE_ASSERT(camera.HasComponent<CameraComponent>(), "Atempt to set valid Entity with no CameraComponent as main camera!");
		}
		m_MainCamera = camera;
	}

	Entity Scene::GetMainCamera()
	{
		return Entity(m_MainCamera, this);
	}

	void Scene::StartPhysics2D()
	{
		// Initialize Box2D World
		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = b2Vec2{ 0.0f, -10.0f };

		// Create Rigidbodies
		m_PhysicsWorld2D = new b2WorldId();
		*m_PhysicsWorld2D = b2CreateWorld(&worldDef);
		{
			auto view = m_Registry.view<TransformComponent, Rigidbody2DComponent, Rigidbody2DStorageComponent>();
			for (auto entityID : view)
			{
				auto [transform, rigidbody, storage] = view.get<TransformComponent, Rigidbody2DComponent, Rigidbody2DStorageComponent>(entityID);

				b2BodyDef bodyDef = b2DefaultBodyDef();
				bodyDef.type = GetBox2DBodyType(rigidbody.Type);
				bodyDef.position.x = transform.Translation.x;
				bodyDef.position.y = transform.Translation.y;
				auto rot = b2ComputeCosSin(transform.Rotation.z);
				bodyDef.rotation.c = rot.cosine;
				bodyDef.rotation.s = rot.sine;

				auto body = b2CreateBody(*m_PhysicsWorld2D, &bodyDef);
				storage.BodyID = body;

				b2Body_SetFixedRotation(body, rigidbody.FixedRotation);
			}
		}

		// Create Box Colliders
		{
			auto view = m_Registry.view<TransformComponent, Rigidbody2DStorageComponent, BoxCollider2DComponent>();

			for (auto entityID : view)
			{
				auto [transform, rigidbody, boxCollider] = view.get<TransformComponent, Rigidbody2DStorageComponent, BoxCollider2DComponent>(entityID);

				b2CosSin rot = b2ComputeCosSin(-boxCollider.Rotation);
				b2Polygon box = b2MakeOffsetBox(
					boxCollider.Size.x * transform.Scale.x / 2.0f, boxCollider.Size.y * transform.Scale.y / 2.0f,
					b2Vec2{ boxCollider.Offset.x, boxCollider.Offset.y },
					b2Rot{ rot.cosine, rot.sine });

				b2ShapeDef shapeDef = b2DefaultShapeDef();
				shapeDef.density = boxCollider.Density;
				shapeDef.friction = boxCollider.Friction;
				shapeDef.restitution = boxCollider.Restitution;
				b2CreatePolygonShape(rigidbody.BodyID, &shapeDef, &box);
			}
		}

		// Create Circle Colliders
		{
			auto view = m_Registry.view<TransformComponent, Rigidbody2DStorageComponent, CircleCollider2DComponent>();

			for (auto entityID : view)
			{
				auto [transform, rigidbody, circleCollider] = view.get<TransformComponent, Rigidbody2DStorageComponent, CircleCollider2DComponent>(entityID);

				b2Circle circle;
				circle.center = b2Vec2(circleCollider.Offset.x, circleCollider.Offset.y);
				circle.radius = circleCollider.Radius * transform.Scale.x;

				b2ShapeDef shapeDef = b2DefaultShapeDef();
				shapeDef.density = circleCollider.Density;
				shapeDef.friction = circleCollider.Friction;
				shapeDef.restitution = circleCollider.Restitution;

				b2CreateCircleShape(rigidbody.BodyID, &shapeDef, &circle);
			}
		}
	}

	void Scene::StopPhysics2D()
	{
		// Destroy Physics world
		b2DestroyWorld(*m_PhysicsWorld2D);
		delete m_PhysicsWorld2D;
		m_PhysicsWorld2D = nullptr;

		// Delete bodyIDs
		{
			auto view = m_Registry.view<Rigidbody2DStorageComponent>();

			for (auto entityID : view)
			{
				view.get<Rigidbody2DStorageComponent>(entityID).BodyID = b2_nullBodyId;
			}
		}
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
		if (m_IsRunning)
		{
			component.Instance = component.InstantiateScript();
			component.Instance->m_Entity = entity;
			component.Instance->OnCreate();
		}
		else
		{
			component.Instance = nullptr;
		}
	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{
		if (!entity.HasComponent<Rigidbody2DStorageComponent>())
		{
			entity.AddComponent<Rigidbody2DStorageComponent>();
		}
	}

}