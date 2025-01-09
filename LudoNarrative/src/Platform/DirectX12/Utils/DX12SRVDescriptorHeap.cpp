#include "ldpch.h"
#include "DX12SRVDescriptorHeap.h"

#include "Platform/DirectX12/Utils/DX12Utils.h"
#include "Platform/DirectX12/DirectX12API.h"

namespace Ludo {

#define LD_SHADER_RESOURCE_VIEW_COUNT 32064 // Hopefully I'll never need to increase the size of this

    bool DX12SRVDescriptorHeap::Init()
    {
        LD_PROFILE_FUNCTION();

        D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
        descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        descHeapDesc.NumDescriptors = LD_SHADER_RESOURCE_VIEW_COUNT; 
        descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        descHeapDesc.NodeMask = 0;

        HRESULT hr = DirectX12API::Get()->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
        VALIDATE_DX12_HRESULT(hr, "Failed to create Descriptor Heap with 32.064 Descriptors");

        m_CpuHeapStart = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        m_GpuHeapStart = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
        m_IncrementAmount = DirectX12API::Get()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        m_ImGuiFreeIndices.reserve(64);
        for (uint32_t i = 0; i < 64; i++)
        {
            m_ImGuiFreeIndices.push_back(i);
        }

        return true;
    }

    void DX12SRVDescriptorHeap::NewFrame()
    {
        LD_PROFILE_RENDERER_FUNCTION();
        
        // Simple cleanup
        m_CurrentOffset = 64;
        m_CurrentMaxSlot = -1;

        DirectX12API::Get()->GetCommandList()->SetDescriptorHeaps(1, &m_DescriptorHeap);
    }

    void DX12SRVDescriptorHeap::BindToDescriptorTable()
    {
        LD_PROFILE_RENDERER_FUNCTION();

        auto handle = m_GpuHeapStart;
        handle.ptr += m_CurrentOffset * m_IncrementAmount;

        DirectX12API::Get()->GetCommandList()->SetGraphicsRootDescriptorTable(2, handle);

        m_CurrentOffset += m_CurrentMaxSlot + 1;
        m_CurrentMaxSlot = -1;
    }

    void DX12SRVDescriptorHeap::CreateDescriptorInOffset(ID3D12Resource2* const& resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, uint32_t slot)
    {
        LD_PROFILE_RENDERER_FUNCTION();

        LD_CORE_ASSERT((m_CurrentOffset + slot) < LD_SHADER_RESOURCE_VIEW_COUNT, "Texture slot is out of Descriptor Heap bounds");

        auto handle = m_CpuHeapStart;
        handle.ptr += (m_CurrentOffset + slot) * m_IncrementAmount;
        DirectX12API::Get()->GetDevice()->CreateShaderResourceView(resource, &srvDesc, handle);

        if ((int32_t)slot > m_CurrentMaxSlot) 
        { 
            m_CurrentMaxSlot = slot; 
        }
    }

    void DX12SRVDescriptorHeap::Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
    {
        LD_CORE_ASSERT(m_ImGuiFreeIndices.size() > 0, "ImGui is triyng to use more SRV's than it should");

        int idx = m_ImGuiFreeIndices.back();
        m_ImGuiFreeIndices.pop_back();
        out_cpu_desc_handle->ptr = m_CpuHeapStart.ptr + (idx * m_IncrementAmount);
        out_gpu_desc_handle->ptr = m_GpuHeapStart.ptr + (idx * m_IncrementAmount);
    }

    void DX12SRVDescriptorHeap::Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle)
    {
        int cpu_idx = (int)((out_cpu_desc_handle.ptr - m_CpuHeapStart.ptr) / m_IncrementAmount);
        int gpu_idx = (int)((out_gpu_desc_handle.ptr - m_GpuHeapStart.ptr) / m_IncrementAmount);
        LD_CORE_ASSERT(cpu_idx == gpu_idx, "Imgui attempted to free a descriptor with unmatching CPU and GPU handles");
        m_ImGuiFreeIndices.push_back(cpu_idx);
    }

    void DX12SRVDescriptorHeap::ShutDown()
    {
        LD_PROFILE_FUNCTION();

        CHECK_AND_RELEASE_COMPTR(m_DescriptorHeap);
    }

}