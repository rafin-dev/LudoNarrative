#pragma once

#include <DirectXMath.h>

#include "Ludo/Renderer/Texture.h"

namespace Ludo {

	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const DirectX::XMFLOAT2& min = { 0.0f, 0.0f }, const DirectX::XMFLOAT2& max = { 1.0f, 1.0f });

		const Ref<Texture2D>& GetTexture() { return m_TextureAtlas; }
		const DirectX::XMFLOAT2* GetTexCoords() { return m_TexCoords; }

		static Ref<SubTexture2D> Create(const Ref<Texture2D>& texture, const DirectX::XMFLOAT2& min = { 0.0f, 0.0f }, const DirectX::XMFLOAT2& max = { 1.0f, 1.0f });
		static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D>& texture, const DirectX::XMFLOAT2& coords, const DirectX::XMFLOAT2& cellSize, const DirectX::XMFLOAT2& spriteSize = { 1.0f, 1.0f });

	private:
		Ref<Texture2D> m_TextureAtlas;

		DirectX::XMFLOAT2 m_TexCoords[4];
	};

}