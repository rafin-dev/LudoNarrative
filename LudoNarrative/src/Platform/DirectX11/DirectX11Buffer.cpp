#include "ldpch.h"
#include "DirectX11Buffer.h"

#include "DirectX11API.h"
#include "Utils/DirectX11Utils.h"

namespace Ludo {

	// ========== Vertex Buffer ==========
	DirectX11VertexBuffer::DirectX11VertexBuffer(float* verticies, uint32_t size, const BufferLayout& layout)
		: m_Layout(layout)
	{
		LD_PROFILE_FUNCTION();

		if (!Init(size))
		{
			return;
		}

		SetData(verticies, size);
	}

	DirectX11VertexBuffer::DirectX11VertexBuffer(uint32_t size, const BufferLayout& layout)
		: m_Layout(layout)
	{
		LD_PROFILE_FUNCTION();

		Init(size);
	}

	bool DirectX11VertexBuffer::Init(uint32_t size)
	{
		LD_PROFILE_FUNCTION();

		auto deviceContext = DirectX11API::Get()->GetDeviceContext();

		m_Size = size;

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = size;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hr = DirectX11API::Get()->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_VertexBuffer);
		VALIDATE_DX_HRESULT(hr, "Failed to create Vertex Buffer of size: {0}", size);

		LD_CORE_TRACE("Created Vertex Buffer of size: {0}", size);

		return true;
	}

	DirectX11VertexBuffer::~DirectX11VertexBuffer()
	{
		ShutDown();
		LD_CORE_TRACE("Deleted Vertex Buffer");
	}

	void DirectX11VertexBuffer::Bind() const
	{
		LD_PROFILE_FUNCTION();

		UINT stride = m_Layout.GetStride();
		UINT offset = 0;
		DirectX11API::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	}

	void DirectX11VertexBuffer::Unbind() const
	{
		LD_PROFILE_FUNCTION();
	}

	void DirectX11VertexBuffer::SetData(void* data, uint32_t size)
	{
		LD_PROFILE_FUNCTION();

		LD_CORE_ASSERT(size <= m_Size, "Vertex Buffer of size {0} bytes cannot have {1} bytes of data written on!", m_Size, size);

		D3D11_MAPPED_SUBRESOURCE mapped = {};
		HRESULT hr = DirectX11API::Get()->GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mapped);
		LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to map Vertes Buffer");
		std::memcpy(mapped.pData, data, size);
		DirectX11API::Get()->GetDeviceContext()->Unmap(m_VertexBuffer, 0);

	}

	void DirectX11VertexBuffer::ShutDown()
	{
		LD_PROFILE_FUNCTION();

		CHECK_AND_RELEASE_COMPTR(m_VertexBuffer);
	}

	// ========== Index Buffer ==========
	DirectX11IndexBuffer::DirectX11IndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		Init(indices, count);
	}

	bool DirectX11IndexBuffer::Init(uint32_t* indices, uint32_t count)
	{
		LD_PROFILE_FUNCTION();

		auto deviceContext = DirectX11API::Get()->GetDeviceContext();
		size_t size = count * 4;

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = size;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hr = DirectX11API::Get()->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_IndexBuffer);
		VALIDATE_DX_HRESULT(hr, "Failed to create Index Buffer of count: {0}", count);

		D3D11_MAPPED_SUBRESOURCE mapped = {};
		hr = deviceContext->Map(m_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mapped);
		VALIDATE_DX_HRESULT(hr, "Failed to Map Index Buffer of count: {0}", count);
		std::memcpy(mapped.pData, indices, size);
		deviceContext->Unmap(m_IndexBuffer, 0);

		LD_CORE_TRACE("Created Index Buffer of count: {0}", count);

		return true;
	}

	DirectX11IndexBuffer::~DirectX11IndexBuffer()
	{
		ShutDown();
		LD_CORE_TRACE("Deleted Index Buffer");
	}

	void DirectX11IndexBuffer::Bind() const
	{
		LD_PROFILE_FUNCTION();

		DirectX11API::Get()->GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	}

	void DirectX11IndexBuffer::Unbind() const
	{
		LD_PROFILE_FUNCTION();
	}

	void DirectX11IndexBuffer::ShutDown()
	{
		LD_PROFILE_FUNCTION();

		CHECK_AND_RELEASE_COMPTR(m_IndexBuffer);
	}

}