#include "ldpch.h"
#include "DirectX11UniformBuffer.h"

#include "Utils/DirectX11Utils.h"
#include "DirectX11API.h"

namespace Ludo {

	DirectX11UniformBuffer::DirectX11UniformBuffer(uint32_t size, uint32_t slot)
		: m_Size(size), m_Slot(slot)
	{
		LD_PROFILE_FUNCTION();

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = size;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;

		HRESULT hr = DirectX11API::Get()->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_Buffer);
		LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to create Uniform Buffer");
	}

	DirectX11UniformBuffer::~DirectX11UniformBuffer()
	{
		LD_PROFILE_FUNCTION();

		CHECK_AND_RELEASE_COMPTR(m_Buffer);
	}

	void DirectX11UniformBuffer::SetData(void* data, size_t size, size_t offset)
	{
		LD_PROFILE_RENDERER_FUNCTION();

		LD_CORE_ASSERT((size + offset) >= m_Size, "Atempt to write out of buffer bounds");

		D3D11_MAPPED_SUBRESOURCE mapped = {};

		HRESULT hr = DirectX11API::Get()->GetDeviceContext()->Map(m_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to map Uniform Buffer");

		std::memcpy((uint8_t*)mapped.pData + offset, data, size);

		DirectX11API::Get()->GetDeviceContext()->Unmap(m_Buffer, 0);
	}

	void DirectX11UniformBuffer::Bind() const
	{
		LD_PROFILE_RENDERER_FUNCTION();

		DirectX11API::Get()->GetDeviceContext()->VSSetConstantBuffers(m_Slot, 1, &m_Buffer);
		DirectX11API::Get()->GetDeviceContext()->PSSetConstantBuffers(m_Slot, 1, &m_Buffer);
	}

}