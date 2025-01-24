#pragma once

#include "Ludo/Renderer/Texture.h"

#include <d3d12.h>

namespace Ludo {

	class DirectX12Texture2D : public Texture2D
	{
	public:
		DirectX12Texture2D(uint32_t width, uint32_t height);
		DirectX12Texture2D(const std::filesystem::path& path);
		virtual ~DirectX12Texture2D() override;

		virtual uint32_t GetWidth() const override;
		virtual uint32_t GetHeight() const override;

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool operator==(const Texture2D& other) const override
		{
			return m_ID == ((DirectX12Texture2D&)other).m_ID;
		}

		ID3D12Resource2* const & GetResource() { return m_Texture; }

	private:
		bool Init();
		void ShutDown();

		ID3D12Resource2* m_Texture = nullptr;

		std::filesystem::path m_Path = "None";
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		uint32_t m_ID;
	};

}