#include "ldpch.h"
#include "DX12UploadBuffer.h"

#include "Platform/DirectX12/Utils/DX12Utils.h"
#include "Platform/DirectX12/DirectX12API.h"

namespace Ludo {

    bool DX12UploadBuffer::Init(size_t size)
    {
        m_Size = size;

        return CreateUploadBuffer();
    }

    void DX12UploadBuffer::ImediateUploadData(ID3D12Resource2*& const destBuffer, void* data, size_t offset, size_t size)
    {
        LD_PROFILE_FUNCTION();

        LD_CORE_ASSERT(size <= (m_Size - offset), "Atempt to write out of Upload Buffer Bounds");

        memcpy((uint8_t*)m_Dest + offset, data, size);

        DirectX12API::Get()->ImediateCopyBufferRegion(destBuffer, offset, m_UploadBuffer, 0, size);
    }

    void DX12UploadBuffer::QueuUploadData(ID3D12Resource2*& const destBuffer, void* data, size_t offset, size_t size)
    {
        LD_PROFILE_FUNCTION();

        LD_CORE_ASSERT(size <= (m_Size - offset), "Atempt to write out of Upload Buffer Bounds");

        memcpy((uint8_t*)m_Dest + offset, data, size);

        DirectX12API::Get()->GetCommandList()->CopyBufferRegion(destBuffer, offset, m_UploadBuffer, 0, size);
    }

    void DX12UploadBuffer::FillBufferData(void* data, size_t offset, uint32_t size)
    {
        LD_PROFILE_FUNCTION();

        LD_CORE_ASSERT(size <= (m_Size - offset), "Atempt to write out of Upload Buffer Bounds");

        memcpy((uint8_t*)m_Dest + offset, data, size);
    }

    bool DX12UploadBuffer::Resize(size_t size)
    {
        m_Size = size;

        return CreateUploadBuffer();
    }

    bool DX12UploadBuffer::CreateUploadBuffer()
    {
        LD_PROFILE_FUNCTION();

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
        CHECK_DX12_HRESULT(hr, "Failed to create Upload Buffer of size {0}", m_Size) { return false; }

        D3D12_RANGE readRange = { 0, 0 };
        hr = m_UploadBuffer->Map(0, nullptr, &m_Dest);
        CHECK_DX12_HRESULT(hr, "Failed to map Upload Buffer of size {0}", m_Size) { return false; }

        return true;
    }

    DX12UploadBuffer::~DX12UploadBuffer()
    {
        ShutDown();
    }

    void DX12UploadBuffer::ShutDown()
    {
        LD_PROFILE_FUNCTION();

        CHECK_AND_RELEASE_COMPTR(m_UploadBuffer);
        m_Dest = nullptr;
        m_Size = 0;
    }

}