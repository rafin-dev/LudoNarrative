#include "ldpch.h"
#include "SubTexture2D.h"

namespace Ludo {

	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const DirectX::XMFLOAT2& min, const DirectX::XMFLOAT2& max)
		: m_TextureAtlas(texture)
	{
		m_TexCoords[0] = { min.x, min.y };
		m_TexCoords[1] = { min.x, max.y };
		m_TexCoords[2] = { max.x, max.y };
		m_TexCoords[3] = { max.x, min.y };
	}

	Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2D>& texture, const DirectX::XMFLOAT2& coords, const DirectX::XMFLOAT2& cellSize, const DirectX::XMFLOAT2& spriteSize)
	{
		float atlasWidth = texture->GetWidth();
		float atlasHeight = texture->GetHeight();

		DirectX::XMFLOAT2 min = { (coords.x * cellSize.x) / atlasWidth, (coords.y * cellSize.y) / atlasHeight };
		DirectX::XMFLOAT2 max = { ((coords.x + spriteSize.x) * cellSize.x) / atlasWidth, ((coords.y + spriteSize.y) * cellSize.y) / atlasHeight };

		return CreateRef<SubTexture2D>(texture, min, max);
	}

}