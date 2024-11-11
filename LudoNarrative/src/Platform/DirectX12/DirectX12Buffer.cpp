#include "ldpch.h"
#include "DirectX12Buffer.h"

#include "DirectX12API.h"
#include "DX12Utils.h"

// Currently, when creating a Vertex or Index Buffer, 2 completely new buffers will be allocated
// A Upload Buffer and a Buffer for the actual resource, theres a lot of optimization to come

// For now, iformation on whether or not the buffer creation was succesfull will not be knwon by the client
// Thats another TODO: btw

namespace Ludo {

	// ========== Vertex Buffer ==========
	DirectX12VertexBuffer::DirectX12VertexBuffer(float* verticies, uint32_t size, const BufferLayout& layout)
		: m_Layout(layout)
	{
		bool result = Init(verticies, size);
		LD_CORE_ASSERT(result, "failed to create Vertex Buffer");
	}

	bool DirectX12VertexBuffer::Init(float* verticies, uint32_t size)
	{
		HRESULT hr = S_OK;
		auto& device = DirectX12API::Get()->GetDevice();

		// ========== Heap & Resource properties ==========
		D3D12_HEAP_PROPERTIES heapPropertiesUpload = {};
		heapPropertiesUpload.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapPropertiesUpload.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapPropertiesUpload.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapPropertiesUpload.CreationNodeMask = 0;
		heapPropertiesUpload.VisibleNodeMask = 0;

		D3D12_HEAP_PROPERTIES heapPropertiesVertex = {};
		heapPropertiesVertex.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapPropertiesVertex.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapPropertiesVertex.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapPropertiesVertex.CreationNodeMask = 0;
		heapPropertiesVertex.VisibleNodeMask = 0;

		// ========== Common Upload & Vertex buffer description ==========
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

		// ========== Upload Buffer ==========
		ID3D12Resource2* UploadBuffer = nullptr;
		hr = device->CreateCommittedResource(&heapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &resourceDescription, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&UploadBuffer));
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to create Upload Buffer for Vertex Buffer");

		// ========== Vertex Buffer ==========
		hr = device->CreateCommittedResource(&heapPropertiesVertex, D3D12_HEAP_FLAG_NONE, &resourceDescription, D3D12_RESOURCE_STATE_COMMON,
			nullptr, IID_PPV_ARGS(&m_VertexBuffer)); // If this fails, the upload buffer wont be deleted, but since this is temporary, it's ok
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to create Vertex Buffer");

		// ========== Vertex Buffer View ==========
		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = size;
		m_VertexBufferView.StrideInBytes = m_Layout.GetStride();

		// ========== Data --> CPU Upload Buffer ==========
		void* uploadAddress = nullptr;
		D3D12_RANGE uploadRange = {};
		uploadRange.Begin = 0;
		uploadRange.End = size - 1;

		hr = UploadBuffer->Map(0, &uploadRange, &uploadAddress); // Same as before, if it fails, the COM object will not be released
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to Map Upload Buffer for Vertex Buffer of size: {0}", size);

		memcpy(uploadAddress, verticies, size);

		UploadBuffer->Unmap(0, &uploadRange);

		// ========== CPU Upload Buffer --> GPU Vertex Buffer ==========
		DirectX12API::Get()->ExecuteCommandListAndWait();
		auto& commandList = DirectX12API::Get()->InitCommandList();

		commandList->CopyBufferRegion(m_VertexBuffer, 0, UploadBuffer, 0, size);

		DirectX12API::Get()->ExecuteCommandListAndWait();
		DirectX12API::Get()->InitCommandList();

		CHECK_AND_RELEASE_COMPTR(UploadBuffer);
		return true;
	}

	DirectX12VertexBuffer::~DirectX12VertexBuffer()
	{
		ShutDown();
	}

	void DirectX12VertexBuffer::Bind() const
	{
		DirectX12API::Get()->GetCommandList()->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	}

	void DirectX12VertexBuffer::ShutDown()
	{
		CHECK_AND_RELEASE_COMPTR(m_VertexBuffer);
	}

	// ========== Index Buffer ==========
	DirectX12IndexBuffer::DirectX12IndexBuffer(uint32_t* indices, uint32_t count)
	{
		bool result = Init(indices, count);
		LD_CORE_ASSERT(result, "Failed to create Index Buffer");
	}

	bool DirectX12IndexBuffer::Init(uint32_t* indices, uint32_t count)
	{
		HRESULT hr = S_OK;
		auto& device = DirectX12API::Get()->GetDevice();
		m_Count = count;
		uint32_t size = count * sizeof(uint32_t);

		// ========== Heap & Resource properties ==========
		D3D12_HEAP_PROPERTIES heapPropertiesUpload = {};
		heapPropertiesUpload.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapPropertiesUpload.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapPropertiesUpload.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapPropertiesUpload.CreationNodeMask = 0;
		heapPropertiesUpload.VisibleNodeMask = 0;

		D3D12_HEAP_PROPERTIES heapPropertiesIndex = {};
		heapPropertiesIndex.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapPropertiesIndex.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapPropertiesIndex.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapPropertiesIndex.CreationNodeMask = 0;
		heapPropertiesIndex.VisibleNodeMask = 0;

		// ========== Upload & vertex buffer ==========
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

		// ========== Upload Buffer ==========
		ID3D12Resource2* UploadBuffer = nullptr;
		hr = device->CreateCommittedResource(&heapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &resourceDescription, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&UploadBuffer));
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to create Upload Buffer for Vertex Buffer");

		// ========== Index Buffer ==========
		hr = device->CreateCommittedResource(&heapPropertiesIndex, D3D12_HEAP_FLAG_NONE, &resourceDescription, D3D12_RESOURCE_STATE_COMMON,
			nullptr, IID_PPV_ARGS(&m_IndexBuffer)); // If this fails, the upload buffer wont be deleted, but since this is temporary, it's ok
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to create Vertex Buffer");

		// ========== Index Buffer View ==========
		m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = size;

		// ========== Data --> CPU Upload Buffer ==========
		void* uploadAddress = nullptr;
		D3D12_RANGE uploadRange = {};
		uploadRange.Begin = 0;
		uploadRange.End = size -1;

		hr = UploadBuffer->Map(0, &uploadRange, &uploadAddress); // Same as before, if it fails, the COM object will not be released
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to Map Upload Buffer for Vertex Buffer of count: {0}", m_Count);

		memcpy(uploadAddress, indices, size);

		UploadBuffer->Unmap(0, &uploadRange);

		// ========== CPU Upload Buffer --> GPU Index Buffer ==========
		DirectX12API::Get()->ExecuteCommandListAndWait();
		auto& commandList = DirectX12API::Get()->InitCommandList();

		commandList->CopyBufferRegion(m_IndexBuffer, 0, UploadBuffer, 0, size);

		DirectX12API::Get()->ExecuteCommandListAndWait();
		DirectX12API::Get()->InitCommandList();

		CHECK_AND_RELEASE_COMPTR(UploadBuffer);
		return true;
	}

	DirectX12IndexBuffer::~DirectX12IndexBuffer()
	{
		ShutDown();
	}

	void DirectX12IndexBuffer::Bind() const
	{
		DirectX12API::Get()->GetCommandList()->IASetIndexBuffer(&m_IndexBufferView);
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