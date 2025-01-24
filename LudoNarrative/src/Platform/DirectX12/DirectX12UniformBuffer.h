#pragma once

#include "Ludo/Renderer/UniformBuffer.h"
#include "Platform/DirectX12/Utils/DX12UploadBuffer.h"

namespace Ludo {

	class DirectX12UniformBuffer : public UniformBuffer
	{
	public:
		DirectX12UniformBuffer(size_t size, uint32_t slot);
		~DirectX12UniformBuffer();

		void SetData(void* data, size_t size, size_t offset) override;

	private:
		size_t m_Size;
		uint32_t m_Slot;
	
		DX12UploadBuffer m_UniformBuffer;
	};

}