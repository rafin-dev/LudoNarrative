#pragma once

#include "Ludo/Core/Core.h"
#include "Ludo/Renderer/OrthographicCamera.h"
#include "Ludo/Renderer/Camera.h"
#include "Ludo/Renderer/Texture.h"
#include "Ludo/Renderer/SubTexture2D.h"
#include "Ludo/Renderer/EditorCamera.h"
#include "Ludo/Scene/Components.h"

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
		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		// ================================

		// ========== Primitives ==========
		// Roation is in radians
		
		// All "static void DrawQuad(...)"s will end calling a "static LD_SIMD_CALLING_CONVENTION DrawQuad(const DirectX::XMMATRIX& transform, ...)"
		// The reaseon why the texture one is private is because it's shared by textures and subTextures
		// So it doens't follow the parameter convention of them and is only internal

		// Colored Quads
		static void DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, float rotation, const DirectX::XMFLOAT4& color);
		static void DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, const DirectX::XMFLOAT4& color);
		static void DrawQuad(const DirectX::XMFLOAT4X4& transform, const DirectX::XMFLOAT4& color);
		static void DrawQuad(const DirectX::XMMATRIX& transform, const DirectX::XMFLOAT4& color);

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
		static void DrawQuad(const DirectX::XMMATRIX& transform, const Ref<Texture2D>& texture,
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
		static void DrawQuad(const DirectX::XMMATRIX& transform, const Ref<SubTexture2D>& subTexture,
			const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			float tilingFactor = 1.0f);
		
		// Circles
		static void DrawCircle(const DirectX::XMFLOAT4X4& transform, const DirectX::XMFLOAT4& color, float thickness, float fade);
		static void DrawCircle(const DirectX::XMMATRIX& transform, const DirectX::XMFLOAT4& color, float thickness, float fade);

		// Sprite
		static void DrawSprite(const DirectX::XMMATRIX& transform, const SpriteRendererComponent& sprite, int entityID);

		// Circle
		static void DrawCircle(const DirectX::XMMATRIX& transform, const CircleRendererComponent& circle, int entityID);
		
		// Line
		static void DrawLine(const DirectX::XMFLOAT3& begin, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT4& color, int entityID = -1);
		static void DrawRect(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT4& color, int entityID = -1);
		static void DrawRect(const DirectX::XMFLOAT4X4& transform, const DirectX::XMFLOAT4& color, int entityID = -1);
		static void DrawRect(const DirectX::XMMATRIX& transform, const DirectX::XMFLOAT4& color, int entityID = -1);

		// ================================

		// ========== Statistics ==========
		struct Statistics
		{
			uint32_t TotalDrawCalls = 0;
			uint32_t QuadDrawCalls = 0;
			uint32_t CircleDrawCalls = 0;
			uint32_t LineDrawCalls = 0;

			uint32_t TotalObjectCount = 0;
			uint32_t QuadCount = 0;
			uint32_t CircleCount = 0;
			uint32_t LineCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		static void ResetStats();
		static Statistics GetStats();
		// ================================

	private:
		// Internal use
		static void DrawQuad(int entityID, const DirectX::XMMATRIX& transform, const DirectX::XMFLOAT4& color);

		static void DrawQuad(int entityID, const DirectX::XMMATRIX& transform, const Ref<SubTexture2D>& subTexture,
			const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			float tilingFactor = 1.0f);
		
		static void DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, 
			const Ref<Texture2D>& texture, const DirectX::XMFLOAT2* texCoords,
			const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			float tilingFactor = 1.0f);

		static void DrawQuad(int entityID, const DirectX::XMMATRIX& transform, const Ref<Texture2D>& texture, const DirectX::XMFLOAT2* texCoords, const DirectX::XMFLOAT4& tintColor,
			float tilingFactor);
		
		static void DrawCircle(int EntityID, const DirectX::XMMATRIX& transform, const DirectX::XMFLOAT4& color, float thickness, float fade);

		static void Flush();
		static void ClearData();
		static void FlushAndReset();
	};

}