#pragma once

#include "Ludo/Renderer/Texture.h"

namespace Ludo {

	class DirectX11Texture2D : public Texture2D
	{
	public:
		DirectX11Texture2D(const std::string& path);
		virtual ~DirectX11Texture2D() override;

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void Bind(uint32_t slot) const override;

	private:
		bool Init();
		void ShutDown();

		std::string m_Path;
		uint32_t m_Width, m_Height;
		
		ID3D11Texture2D* m_Texture = nullptr;
		ID3D11ShaderResourceView* m_ShaderResourceView = nullptr;
		ID3D11SamplerState* m_SamplerState = nullptr;
	};

}