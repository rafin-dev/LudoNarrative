#include "ldpch.h"
#include "DirectX12Buffer.h"

#include "Platform/DirectX12/DirectX12API.h"
#include "Platform/DirectX12/Utils/DX12Utils.h"

namespace Ludo {

	// ===================================
	// ========== Vertex Buffer ==========
	// ===================================

	DirectX12VertexBuffer::DirectX12VertexBuffer(float* verticies, uint32_t size, const BufferLayout& layout)
		: m_Layout(layout)
	{
		LD_PROFILE_FUNCTION();

		bool result = Init(size);
		LD_CORE_ASSERT(result, "Failed to create Vertex Buffer");

		SetData(verticies, size);
	}

	DirectX12VertexBuffer::DirectX12VertexBuffer(uint32_t size, const BufferLayout& layout)
		: m_Layout(layout)
	{
		LD_PROFILE_FUNCTION();

		bool result = Init(size);
		LD_CORE_ASSERT(result, "Failed to create Vertex Buffer");
	}

	DirectX12VertexBuffer::~DirectX12VertexBuffer()
	{
		ShutDown();
	}

	void DirectX12VertexBuffer::Bind() const
	{
		ImplBind(0);
	}

	void DirectX12VertexBuffer::ImplBind(uint32_t slot) const
	{
		LD_PROFILE_FUNCTION();

		DirectX12API::Get()->GetCommandList()->IASetVertexBuffers(slot, 1, &m_VertexBufferView);
	}

	void DirectX12VertexBuffer::Unbind() const
	{
	}

	void DirectX12VertexBuffer::SetData(void* data, uint32_t size)
	{
		LD_PROFILE_FUNCTION();

		m_UploadBuffer.ImediateUploadData(m_VertexBuffer, data, 0, size);
	}

	bool DirectX12VertexBuffer::Init(size_t size)
	{
		LD_PROFILE_FUNCTION();

		HRESULT hr = S_OK;
		auto& device = DirectX12API::Get()->GetDevice();

		// ========== Heap & Resource properties ==========
		D3D12_HEAP_PROPERTIES heapPropertiesVertex = {};
		heapPropertiesVertex.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapPropertiesVertex.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapPropertiesVertex.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapPropertiesVertex.CreationNodeMask = 0;
		heapPropertiesVertex.VisibleNodeMask = 0;

		// ========== Vertex buffer description ==========
		D3D12_RESOURCE_DESC resourceDescription = {};
		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDescription.Width = size;
		resourceDescription.Height = 1;
		resourceDescription.DepthOrArraySize = 1;
		resourceDescription.MipLevels = 1;
		resourceDescription.Format = DXGI_FORMAT_UNKNOWN;
		resourceDescription.SampleDesc.Count = 1;
		resourceDescription.SampleDesc.Quality = 0;
		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

		// ========== Vertex Buffer ==========
		hr = device->CreateCommittedResource(&heapPropertiesVertex, D3D12_HEAP_FLAG_NONE, &resourceDescription, D3D12_RESOURCE_STATE_COMMON,
			nullptr, IID_PPV_ARGS(&m_VertexBuffer));
		VALIDATE_DX12_HRESULT(hr, "Failed to create Vertex Buffer");

		// ========== Vertex Buffer View ==========
		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = size;
		m_VertexBufferView.StrideInBytes = m_Layout.GetStride();

		m_UploadBuffer.Init(size);

		return true;
	}

	void DirectX12VertexBuffer::ShutDown()
	{
		LD_PROFILE_FUNCTION();

		CHECK_AND_RELEASE_COMPTR(m_VertexBuffer);
	}

	// ==================================
	// ========== Index Buffer ==========
	// ==================================

	DirectX12IndexBuffer::DirectX12IndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		LD_PROFILE_FUNCTION();

		bool result = Init(indices, count);
		LD_CORE_ASSERT(result, "Failed to create Index Buffer");
	}

	bool DirectX12IndexBuffer::Init(uint32_t* indices, uint32_t count)
	{
		LD_PROFILE_FUNCTION();

		HRESULT hr = S_OK;
		auto& device = DirectX12API::Get()->GetDevice();
		m_Count = count;
		uint32_t size = count * sizeof(uint32_t);

		// ========== Heap & Resource Properties ==========
		D3D12_HEAP_PROPERTIES heapPropertiesIndex = {};
		heapPropertiesIndex.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapPropertiesIndex.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapPropertiesIndex.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapPropertiesIndex.CreationNodeMask = 0;
		heapPropertiesIndex.VisibleNodeMask = 0;

		// ========== Index Buffer Properties ==========
		D3D12_RESOURCE_DESC resourceDescription = {};
		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDescription.Width = size;
		resourceDescription.Height = 1;
		resourceDescription.DepthOrArraySize = 1;
		resourceDescription.MipLevels = 1;
		resourceDescription.Format = DXGI_FORMAT_UNKNOWN;
		resourceDescription.SampleDesc.Count = 1;
		resourceDescription.SampleDesc.Quality = 0;
		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

		// ========== Index Buffer ==========
		hr = device->CreateCommittedResource(&heapPropertiesIndex, D3D12_HEAP_FLAG_NONE, &resourceDescription, D3D12_RESOURCE_STATE_COMMON,
			nullptr, IID_PPV_ARGS(&m_IndexBuffer));
		VALIDATE_DX12_HRESULT(hr, "Failed to create Index Buffer of size {0}", count);

		// ========== Index Buffer View ==========
		m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = size;

		DX12UploadBuffer upBuffer;
		upBuffer.Init(size);
		upBuffer.ImediateUploadData(m_IndexBuffer, indices, 0, count * sizeof(uint32_t));

		return true;
	}

	DirectX12IndexBuffer::~DirectX12IndexBuffer()
	{
		ShutDown();
	}

	void DirectX12IndexBuffer::Bind() const
	{
		LD_PROFILE_FUNCTION();

		DirectX12API::Get()->GetCommandList()->IASetIndexBuffer(&m_IndexBufferView);
	}

	void DirectX12IndexBuffer::Unbind() const
	{
	}

	uint32_t DirectX12IndexBuffer::GetCount() const
	{
		return m_Count;
	}

	void DirectX12IndexBuffer::ShutDown()
	{
		CHECK_AND_RELEASE_COMPTR(m_IndexBuffer);
	}

}