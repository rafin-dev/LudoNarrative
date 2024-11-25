#include "ldpch.h"
#include "DirectX12UploadBuffer.h"

#include "Platform/DirectX12/DirectX12API.h"
#include "DX12Utils.h"

namespace Ludo {

	DirectX12UploadBuffer::DirectX12UploadBuffer(size_t sizeInBytes)
		: m_Size(sizeInBytes)
	{
		CreateUploadBuffer();
	}

	DirectX12UploadBuffer::~DirectX12UploadBuffer()
	{
		ShutDown();
	}

	bool DirectX12UploadBuffer::UploadData(ID3D12Resource2*& const destBuffer, void* data)
	{
		memcpy(m_Dest, data, m_Size);

		DirectX12API::Get()->ExecuteCommandListAndWait();
		auto& commandList = DirectX12API::Get()->InitCommandList();

		commandList->CopyBufferRegion(destBuffer, 0, m_UploadBuffer, 0, m_Size);

		DirectX12API::Get()->ExecuteCommandListAndWait();
		DirectX12API::Get()->InitCommandList();

		return false;
	}

	bool DirectX12UploadBuffer::Resize(size_t newSizeInBytes)
	{
		m_Size = newSizeInBytes;
		return CreateUploadBuffer();
	}

	bool DirectX12UploadBuffer::CreateUploadBuffer()
	{
		// ========== Heap & Resource properties ==========
		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 0;
		heapProperties.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC resourceDescription = {};
		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDescription.Width = m_Size;
		resourceDescription.Height = 1;
		resourceDescription.DepthOrArraySize = 1;
		resourceDescription.MipLevels = 1;
		resourceDescription.Format = DXGI_FORMAT_UNKNOWN;
		resourceDescription.SampleDesc.Count = 1;
		resourceDescription.SampleDesc.Quality = 0;
		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

		HRESULT hr = DirectX12API::Get()->GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDescription, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&m_UploadBuffer));
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to create upload buffer of size: {0}", m_Size);

		D3D12_RANGE range = { 0, 0 };

		if (FAILED(m_UploadBuffer->Map(0, nullptr, &m_Dest)))
		{
			LD_CORE_ERROR("Failed to map Upload Buffer of size: {0}", m_Size);
			return false;
		}

		return true;
	}

	void DirectX12UploadBuffer::ShutDown()
	{
		CHECK_AND_RELEASE_COMPTR(m_UploadBuffer);
	}

}