#pragma once

#include "Ludo/Renderer/UniformBuffer.h"

#include <d3d11.h>

namespace Ludo {

	class DirectX11UniformBuffer : public UniformBuffer
	{
	public:
		DirectX11UniformBuffer(uint32_t size, uint32_t slot);
		~DirectX11UniformBuffer() override;

		void SetData(void* data, size_t size, size_t offset) override;

		void Bind() const override;

	private:
		ID3D11Buffer* m_Buffer = nullptr;
		uint32_t m_Size = 0;
		uint32_t m_Slot = 0;
	};

}