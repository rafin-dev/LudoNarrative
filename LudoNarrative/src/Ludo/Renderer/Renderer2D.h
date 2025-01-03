#pragma once

#include "Ludo/Core/Core.h"
#include "Ludo/Renderer/OrthographicCamera.h"
#include "Ludo/Renderer/Texture.h"

namespace Ludo {

	class Renderer2D
	{
	public:
		// ========== System ==========
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		// ========== Primitives ==========
		static void DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, float rotation, const DirectX::XMFLOAT4& color);
		static void DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, const DirectX::XMFLOAT4& color);
	
		static void DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<Texture2D>& texture, 
			const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			float tilingFactor = 1.0f);
		static void DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<Texture2D>& texture, 
			const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			float tilingFactor = 1.0f);

		// ========== Statistics ==========
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		static void ResetStats();
		static Statistics GetStats();

	private:
		static void FlushAndReset();
	};

}