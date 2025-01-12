#pragma once

#include "Ludo/Core/Core.h"
#include "Ludo/Renderer/OrthographicCamera.h"
#include "Ludo/Renderer/Camera.h"
#include "Ludo/Renderer/Texture.h"
#include "Ludo/Renderer/SubTexture2D.h"

namespace Ludo {

	class Renderer2D
	{
	public:
		struct RenderCamera
		{
			DirectX::XMFLOAT4X4 Projection;
			DirectX::XMFLOAT4X4 View;
		};

		// ========== System =============
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const DirectX::XMFLOAT4X4& transform);
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();
		// ================================

		// ========== Primitives ==========
		// Roation is in radians

		// Colored Quads
		static void DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, float rotation, const DirectX::XMFLOAT4& color);
		static void DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, const DirectX::XMFLOAT4& color);
		static void DrawQuad(const DirectX::XMFLOAT4X4& transform, const DirectX::XMFLOAT4& color);

		// Textured Quads
		static void DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<Texture2D>& texture, 
			const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			float tilingFactor = 1.0f);
		static void DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<Texture2D>& texture, 
			const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			float tilingFactor = 1.0f);
		static void DrawQuad(const DirectX::XMFLOAT4X4& transform, const Ref<Texture2D>& texture,
			const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			float tilingFactor = 1.0f);

		// SubTextured Quads
		static void DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<SubTexture2D>& subTexture,
			const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			float tilingFactor = 1.0f);
		static void DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<SubTexture2D>& subTexture,
			const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			float tilingFactor = 1.0f);
		static void DrawQuad(const DirectX::XMFLOAT4X4& transform, const Ref<SubTexture2D>& subTexture,
			const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			float tilingFactor = 1.0f);
		// ================================

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
		// ================================

	private:
		// Internal use
		static void DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, 
			const Ref<Texture2D>& texture, const DirectX::XMFLOAT2* texCoords,
			const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			float tilingFactor = 1.0f);

		// The "__fastcall" is windows x64 convetions to properly pass SIMD aligned parameters
		// TODO: Add a macro that chages depending on the target platform
		static void __fastcall DrawQuad(const DirectX::XMMATRIX& transform, const DirectX::XMFLOAT4& color);
		static void __fastcall DrawQuad(const DirectX::XMMATRIX& transform, const Ref<Texture2D>& texture, const DirectX::XMFLOAT2* texCoords, const DirectX::XMFLOAT4& tintColor,
			float tilingFactor);
		
		static void FlushAndReset();
	};

}