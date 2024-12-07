#pragma once

#include "Ludo/Core/Core.h"
#include "OrthographicCamera.h"

namespace Ludo {

	class Renderer2D
	{
	public:
		// ========== System ==========
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		// ========== Primitives ==========
		static void DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT4& color);
		static void DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT4& color);
	};

}