#include "ldpch.h"
#include "DirectX12Buffer.h"

#include "Platform/DirectX12/DirectX12API.h"
#include "Platform/DirectX12/Utils/DX12Utils.h"

namespace Ludo {

	// ===================================
	// ========== Vertex Buffer ==========
	// ===================================

	DirectX12VertexBuffer::DirectX12VertexBuffer(float* verticies, uint32_t size, const BufferLayout& layout, VBUpdateFrequency updateFrequency)
		: m_Layout(layout)
	{
		LD_PROFILE_FUNCTION();

		bool result = Init(size, updateFrequency);
		LD_CORE_ASSERT(result, "Failed to create Vertex Buffer");

		// If its a Immutable Buffer, SetData wouldn't do anything, so we set the data manually
		if (updateFrequency == VBUpdateFrequency::IMMUTABLE)
		{
			DX12UploadBuffer upBuffer;
			upBuffer.Init(size);

			auto& commandList = DirectX12API::Get()->GetCopyCommandHelper().InitCommandList();
			upBuffer.FillBufferData(verticies, 0, size);
			
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = m_VertexBuffer;
			barrier.Transition.Subresource = 0;

			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

			commandList->ResourceBarrier(1, &barrier);
			commandList->CopyBufferRegion(m_VertexBuffer, 0, upBuffer.GetBuffer(), 0, size);

			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			commandList->ResourceBarrier(1, &barrier);
			
			return;
		}

		SetData(verticies, size);
	}

	DirectX12VertexBuffer::DirectX12VertexBuffer(uint32_t size, const BufferLayout& layout, VBUpdateFrequency updateFrequency)
		: m_Layout(layout)
	{
		LD_PROFILE_FUNCTION();

		LD_CORE_ASSERT(updateFrequency != VBUpdateFrequency::IMMUTABLE, "A Immutable Vertex Buffer cannot be initialized withot any data");

		bool result = Init(size, updateFrequency);
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
		LD_PROFILE_RENDERER_FUNCTION();

		DirectX12API::Get()->GetCommandList()->IASetVertexBuffers(slot, 1, &m_VertexBufferView);
	}

	void DirectX12VertexBuffer::Unbind() const
	{
	}

	void DirectX12VertexBuffer::SetData(void* data, uint32_t size)
	{
		LD_PROFILE_RENDERER_FUNCTION();

		m_Updater->UpdateVertexBuffer(m_VertexBuffer, 0, data, size);
	}

	bool DirectX12VertexBuffer::Init(size_t size, VBUpdateFrequency updateFrequency)
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
		hr = device->CreateCommittedResource(&heapPropertiesVertex, D3D12_HEAP_FLAG_NONE, &resourceDescription, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
			nullptr, IID_PPV_ARGS(&m_VertexBuffer));
		VALIDATE_DX12_HRESULT(hr, "Failed to create Vertex Buffer");

		// ========== Vertex Buffer View ==========
		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = size;
		m_VertexBufferView.StrideInBytes = m_Layout.GetStride();

		m_Updater = DX12VertexBufferUpdater::Create(updateFrequency);
		if (!m_Updater->Init(size))
		{
			return false;
		}

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
		hr = device->CreateCommittedResource(&heapPropertiesIndex, D3D12_HEAP_FLAG_NONE, &resourceDescription, D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr, IID_PPV_ARGS(&m_IndexBuffer));
		VALIDATE_DX12_HRESULT(hr, "Failed to create Index Buffer of size {0}", count);

		// ========== Index Buffer View ==========
		m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = size;

		DX12UploadBuffer upBuffer;
		upBuffer.Init(size);

		auto& commandList = DirectX12API::Get()->GetCopyCommandHelper().InitCommandList();
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = m_IndexBuffer;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
		barrier.Transition.Subresource = 0;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

		upBuffer.FillBufferData(indices, 0, size);
		commandList->CopyBufferRegion(m_IndexBuffer, 0, upBuffer.GetBuffer(), 0, size);
		commandList->ResourceBarrier(1, &barrier);

		DirectX12API::Get()->GetCopyCommandHelper().ExecuteCommandListAndWait();

		return true;
	}

	DirectX12IndexBuffer::~DirectX12IndexBuffer()
	{
		ShutDown();
	}

	void DirectX12IndexBuffer::Bind() const
	{
		LD_PROFILE_RENDERER_FUNCTION();

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
		LD_PROFILE_FUNCTION();

		CHECK_AND_RELEASE_COMPTR(m_IndexBuffer);
	}

}