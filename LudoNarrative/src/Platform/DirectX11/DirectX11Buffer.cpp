#include "ldpch.h"
#include "DirectX11Buffer.h"

#include "DirectX11API.h"
#include "Utils/DirectX11Utils.h"

namespace Ludo {

	// ========== Vertex Buffer ==========
	DirectX11VertexBuffer::DirectX11VertexBuffer(float* verticies, uint32_t size, const BufferLayout& layout)
		: m_Layout(layout)
	{
		Init(verticies, size);
	}

	bool DirectX11VertexBuffer::Init(float* verticies, uint32_t size)
	{
		auto deviceContext = DirectX11API::Get()->GetDeviceContext();

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = size;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hr = DirectX11API::Get()->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_VertexBuffer);
		VALIDATE_DX_HRESULT(hr, "Failed to create Vertex Buffer of size: {0}", size);

		D3D11_MAPPED_SUBRESOURCE mapped = {};
		hr = deviceContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mapped);
		VALIDATE_DX_HRESULT(hr, "Failed to Map Vertex Buffer of size: {0}", size);
		memcpy(mapped.pData, verticies, size);
		deviceContext->Unmap(m_VertexBuffer, 0);

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
		UINT stride = m_Layout.GetStride();
		UINT offset = 0;
		DirectX11API::Get()->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	}

	void DirectX11VertexBuffer::Unbind() const
	{
	}

	void DirectX11VertexBuffer::ShutDown()
	{
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
		memcpy(mapped.pData, indices, size);
		deviceContext->Unmap(m_IndexBuffer, 0);

		LD_CORE_TRACE("Created Index Buffer of count: {0}", count);

		return true;
	}

	DirectX11IndexBuffer::~DirectX11IndexBuffer()
	{
		ShutDown();
	}

	void DirectX11IndexBuffer::Bind() const
	{
		DirectX11API::Get()->GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	}

	void DirectX11IndexBuffer::Unbind() const
	{
	}

	void DirectX11IndexBuffer::ShutDown()
	{
		CHECK_AND_RELEASE_COMPTR(m_IndexBuffer);
		LD_CORE_TRACE("Deleted Index Buffer");
	}

}