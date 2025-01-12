#pragma once

#include "Ludo/Scene/SceneCamera.h"

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
		DirectX::XMFLOAT4X4 Transform;

		TransformComponent()
		{
			DirectX::XMStoreFloat4x4(&Transform, DirectX::XMMatrixIdentity());
		}
		TransformComponent(const TransformComponent& other) = default;
		TransformComponent(const DirectX::XMFLOAT4X4& transfrom)
			: Transform(transfrom) {}

		operator DirectX::XMFLOAT4X4& () { return Transform; }
		operator const DirectX::XMFLOAT4X4& () const { return Transform; }
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

}