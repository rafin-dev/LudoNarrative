#pragma once

#include "Ludo/Core/Core.h"
#include "Ludo/Scene/SceneCamera.h"
#include "Ludo/Scene/ScriptableEntity.h"
#include "Ludo/Core/TimeStep.h"

#include <DirectXMath.h>

namespace Ludo {

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
		DirectX::XMFLOAT4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent& other) = default;
		SpriteRendererComponent(const DirectX::XMFLOAT4& color)
			: Color(color) {}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& other) = default;
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void(*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			static_assert(std::is_base_of_v<ScriptableEntity, T>);

			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

}