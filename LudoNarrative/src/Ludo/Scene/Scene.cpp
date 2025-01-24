#include "ldpch.h"
#include "Scene.h"

#include "Ludo/Renderer/Renderer2D.h"
#include "Ludo/Scene/Entity.h"
#include "Ludo/Scene/ScriptableEntity.h"
#include "Ludo/Scene/Components.h"

#include <DirectXMath.h>

namespace Ludo {

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{

	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity(m_Registry.create(), this);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateRuntime(TimeStep ts)
	{
		// Update Scripts
		{
			auto group = m_Registry.group<NativeScriptComponent>();
			for (auto entity : group)
			{
				auto& nsc = group.get<NativeScriptComponent>(entity);

				if (nsc.Instance == nullptr)
				{
					nsc.Instance = (ScriptableEntity*)nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity(entity, this);
					nsc.Instance->OnCreate();
				}

				nsc.Instance->OnUpdate(ts);
			}
		}

		// Render 2D Sprites
		
		// Get main camera
		Camera* mainCamera = nullptr;
		DirectX::XMFLOAT4X4 cameraTransform;
		{
			auto group = m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
			for (auto entity : group)
			{
				auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, cameraTransform);

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

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			if (view.get<CameraComponent>(entity).Primary)
			{
				return Entity(entity, this);
			}
		}

		return Entity();
	}

#define LD_BLANK_COMPONENT_ADD_EVENT(x) template<>\
										 void Scene::OnComponentAdded(Entity entity, x& component) {}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}

	LD_BLANK_COMPONENT_ADD_EVENT(TagComponent);

	LD_BLANK_COMPONENT_ADD_EVENT(TransformComponent);

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	LD_BLANK_COMPONENT_ADD_EVENT(SpriteRendererComponent);

	LD_BLANK_COMPONENT_ADD_EVENT(NativeScriptComponent);

}