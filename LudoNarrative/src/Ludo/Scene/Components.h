#pragma once

#include "Ludo/Core/Core.h"
#include "Ludo/Scene/SceneCamera.h"
#include "Ludo/Core/TimeStep.h"
#include "Ludo/Renderer/SubTexture2D.h"
#include "Ludo/Core/UUID.h"

#include <DirectXMath.h>

namespace Ludo {

	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(const UUID& uuid)
			: ID(uuid) { }
	};

	struct TagComponent
	{
		std::string Tag = "None";

		TagComponent() = default;
		TagComponent(const TagComponent& other) = default;
		TagComponent(const std::string& tag)
		: Tag(tag) { }
	};

	struct TransformComponent
	{
		DirectX::XMFLOAT3 Translation = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 Rotation = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = default;
		TransformComponent(const DirectX::XMFLOAT3& translation)
			: Translation(translation) {}

		DirectX::XMFLOAT4X4 GetTransform() const
		{
			DirectX::XMFLOAT4X4 transform;
			DirectX::XMStoreFloat4x4(&transform,
				DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z) *
				DirectX::XMMatrixRotationX(Rotation.x)* DirectX::XMMatrixRotationY(Rotation.y)* DirectX::XMMatrixRotationZ(Rotation.z)*
				DirectX::XMMatrixTranslation(Translation.x, Translation.y, Translation.z));
			return transform;
		}

		void LD_SIMD_CALLING_CONVENTION GetTransform(DirectX::XMMATRIX* transformOutput) // I really dont know if I need this "__fastcall", but I use it just in case
		{
			*transformOutput =
				DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z) *
				DirectX::XMMatrixRotationX(Rotation.x) * DirectX::XMMatrixRotationY(Rotation.y) * DirectX::XMMatrixRotationZ(Rotation.z) *
				DirectX::XMMatrixTranslation(Translation.x, Translation.y, Translation.z);
		}
	};

	struct SpriteRendererComponent
	{
		Ref<SubTexture2D> Texture;
		DirectX::XMFLOAT4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float TilingFactor = 1.0f;
		std::filesystem::path TexturePath = "None";

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent& other) = default;
		SpriteRendererComponent(const DirectX::XMFLOAT4& color)
			: Color(color) {
		}
		SpriteRendererComponent(const Ref<SubTexture2D>& texture, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f }, float tilingFactor = 1.0f)
			: Color(color), Texture(texture), TilingFactor(tilingFactor) {}
	};

	struct CircleRendererComponent
	{
		DirectX::XMFLOAT4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent& other) = default;
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& other) = default;
	};

	class ScriptableEntity;
	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)() = nullptr;
		void(*DestroyScript)(NativeScriptComponent*) = nullptr;

		template<typename T>
		void Bind()
		{
			static_assert(std::is_base_of_v<ScriptableEntity, T>);

			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	// Physics

	struct Rigidbody2DComponent
	{
		enum class BodyType
		{
			Static = 0,
			Dynamic,
			Kinematic
		};

		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		DirectX::XMFLOAT2 Offset = { 0.0f, 0.0f };
		DirectX::XMFLOAT2 Size = { 1.0f, 1.0f };
		float Rotation = 0.0f;

		// TODO: Move into physicsMaterial
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct CircleCollider2DComponent
	{
		DirectX::XMFLOAT2 Offset = { 0.0f, 0.0f };
		float Radius = 0.5f;

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

}