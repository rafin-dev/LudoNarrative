#pragma once

#include "Ludo/Renderer/Texture.h"

#include "Ludo/Renderer/ImGuiTexture.h"

#include <d3d11.h>

namespace Ludo {

	class DirectX11Texture2D : public Texture2D
	{
	public:
		DirectX11Texture2D(uint32_t width, uint32_t height);
		DirectX11Texture2D(const std::filesystem::path& path);
		~DirectX11Texture2D() override;

		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

		void SetData(void* data, uint32_t size) override;

		void Bind(uint32_t slot) const override;

		ImTextureID GetImTextureID() override { return (ImTextureID)m_ShaderResourceView; }

		bool operator==(const Texture2D& other) const override { return m_Texture == ((DirectX11Texture2D&)other).m_Texture; }

	private:
		bool Init(void* data, int channelCount);
		void ShutDown();

		std::filesystem::path m_Path = "None";
		uint32_t m_Width = 0, m_Height = 0;

		ID3D11Texture2D* m_Texture = nullptr;
		ID3D11ShaderResourceView* m_ShaderResourceView = nullptr;
		ID3D11SamplerState* m_SamplerState = nullptr;
	};

	class DirectX11ImGuiTexture : public ImGuiTexture
	{
	public:
		DirectX11ImGuiTexture(const Ref<Texture2D> texture)
		{
			m_Texture = SubTexture2D::Create(texture);
		}
		DirectX11ImGuiTexture(const Ref<SubTexture2D> texture)
		{
			m_Texture = texture;
		}

		Ref<SubTexture2D> GetSubTexture() override
		{
			return m_Texture;
		}

		Ref<Texture2D> GetTexture() override
		{
			return m_Texture->GetTexture();
		}

		ImTextureID GetImTextureID()
		{
			return m_Texture->GetTexture()->GetImTextureID();
		}

	private:
		Ref<SubTexture2D> m_Texture;
	};

}