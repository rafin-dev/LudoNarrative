#include "ldpch.h"
#include "DirectX12UniformBuffer.h"

#include "Platform/DirectX12/DirectX12API.h"

namespace Ludo {

	DirectX12UniformBuffer::DirectX12UniformBuffer(size_t size, uint32_t slot)
		: m_Size(size), m_Slot(slot)
	{
		m_UniformBuffer.Init(m_Size);
	}

	DirectX12UniformBuffer::~DirectX12UniformBuffer()
	{
		
	}

	void DirectX12UniformBuffer::SetData(void* data, size_t size, size_t offset)
	{
		m_UniformBuffer.FillBufferData(data, offset, size);

		DirectX12API::Get()->GetCommandList()->SetGraphicsRootConstantBufferView(m_Slot, m_UniformBuffer.GetBuffer()->GetGPUVirtualAddress());
	}

}