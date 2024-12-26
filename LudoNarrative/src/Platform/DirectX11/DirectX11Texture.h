#pragma once

#include "Ludo/Renderer/Texture.h"

namespace Ludo {

	class DirectX11Texture2D : public Texture2D
	{
	public:
		DirectX11Texture2D(uint32_t width, uint32_t height);
		DirectX11Texture2D(const std::string& path);
		virtual ~DirectX11Texture2D() override;

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot) const override;

		virtual bool operator==(const Texture2D& other) const override
		{
			return m_ID == ((DirectX11Texture2D&)other).m_ID;
		}

	private:
		bool Init(void* data, int channelCount);
		void ShutDown();

		std::string m_Path = "None";
		uint32_t m_Width = 0, m_Height = 0;
		
		ID3D11Texture2D* m_Texture = nullptr;
		ID3D11ShaderResourceView* m_ShaderResourceView = nullptr;
		ID3D11SamplerState* m_SamplerState = nullptr;

		uint32_t m_ID;
	};

}