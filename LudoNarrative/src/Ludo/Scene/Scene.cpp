#include "ldpch.h"
#include "Scene.h"

#include "Ludo/Renderer/Renderer2D.h"
#include "Ludo/Scene/Entity.h"
#include "Ludo/Scene/ScriptableEntity.h"

#include <DirectXMath.h>

#include <box2d/box2d.h>
#include <box2d/collision.h>

namespace Ludo {

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
		if (m_PhysicsWorld2D != nullptr)
		{
			b2DestroyWorld(*m_PhysicsWorld2D);
		}
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

	// Internal components not exposed to the editor, used only to store Box2D IDs
	struct Rigidbody2DStorageComponent
	{
		b2BodyId BodyID;
	};

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

		// Initialize Box2D World
		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = b2Vec2{ 0.0f, -10.0f };

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

		{
			auto view = m_Registry.view<TransformComponent, Rigidbody2DStorageComponent, BoxCollider2DComponent>();

			for (auto entityID : view)
			{
				auto [transform, rigidbody, boxCollider] = view.get<TransformComponent, Rigidbody2DStorageComponent, BoxCollider2DComponent>(entityID);

				b2CosSin rot = b2ComputeCosSin(boxCollider.Rotation);
				b2Polygon box = b2MakeOffsetBox(
					boxCollider.Size.x * transform.Scale.x, boxCollider.Size.y * transform.Scale.y,
					b2Vec2{ boxCollider.Offset.x, boxCollider.Offset.y },
					b2Rot{ rot.cosine, rot.sine });

				b2ShapeDef shapeDef = b2DefaultShapeDef();
				shapeDef.density = boxCollider.Density;
				shapeDef.friction = boxCollider.Friction;
				shapeDef.restitution = boxCollider.Restitution;
				b2CreatePolygonShape(rigidbody.BodyID, &shapeDef, &box);
			}
		}
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

		// Destroy Physics world
		b2DestroyWorld(*m_PhysicsWorld2D);
		delete m_PhysicsWorld2D;
		m_PhysicsWorld2D = nullptr;
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
				transform.Rotation.z = std::acosf(rot.c);
			}
		}

		// Render 2D Sprites
		Entity mainCamera(m_MainCamera, this);
		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera.GetComponent<CameraComponent>().Camera, mainCamera.GetComponent<TransformComponent>().GetTransform());

			auto group = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
			for (auto entityID : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entityID);
				DirectX::XMMATRIX spriteTransform;
				transform.GetTransform(&spriteTransform);

				Renderer2D::DrawSprite(spriteTransform, sprite, (int)entityID);
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdateEditor(TimeStep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		auto group = m_Registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			DirectX::XMMATRIX spriteTransform;
			transform.GetTransform(&spriteTransform);

			Renderer2D::DrawSprite(spriteTransform, sprite, (int)entity);
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