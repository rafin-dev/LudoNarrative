#include "ldpch.h"
#include "DX12CommandHelper.h"

#include "Platform/DirectX12/DirectX12API.h"
#include "Platform/DirectX12/Utils/DX12Utils.h"

namespace Ludo {

    bool DX12CommandHelper::Init()
    {
        LD_PROFILE_FUNCTION();

        auto& device = DirectX12API::Get()->GetDevice();
        HRESULT hr = S_OK;

        D3D12_COMMAND_QUEUE_DESC cmdDesc = {};
        cmdDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        cmdDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        cmdDesc.NodeMask = 0;
        cmdDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        hr = device->CreateCommandQueue(&cmdDesc, IID_PPV_ARGS(&m_CommandQueue));
        VALIDATE_DX12_HRESULT(hr, "Failed to create the D3D12 CommandQueue");

        hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));
        VALIDATE_DX12_HRESULT(hr, "Failed to create D3D12 Command Allocator");

        hr = device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_CommandList));
        VALIDATE_DX12_HRESULT(hr, "Failed to create D3D12 Command List");

        hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
        VALIDATE_DX12_HRESULT(hr, "Failed to create D3D12 Fence");

        m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);
        VALIDATE_DX12_HRESULT(m_FenceEvent != nullptr ? S_OK : E_FAIL, "Failed to create Fence Event");

        return true;
    }

    void DX12CommandHelper::ShutDown()
    {
        LD_PROFILE_FUNCTION();

        CHECK_AND_RELEASE_COMPTR(m_CommandList);
        CHECK_AND_RELEASE_COMPTR(m_CommandAllocator);
        CHECK_AND_RELEASE_COMPTR(m_CommandQueue);

        CHECK_AND_RELEASE_COMPTR(m_Fence);
        if (m_FenceEvent != nullptr)
        {
            CloseHandle(m_FenceEvent);
        }
    }

    ID3D12GraphicsCommandList6* const& DX12CommandHelper::InitCommandList()
    {
        LD_PROFILE_RENDERER_FUNCTION();

        HRESULT hr = m_CommandAllocator->Reset();
        CHECK_DX12_HRESULT(hr, "Failed to Reset Command Allocator");

        hr = m_CommandList->Reset(m_CommandAllocator, nullptr);
        CHECK_DX12_HRESULT(hr, "Failed to Reset Command List");

        return m_CommandList;
    }

    void DX12CommandHelper::Flush(uint32_t count)
    {
        LD_PROFILE_RENDERER_FUNCTION();

        for (int i = 0; i < count; i++)
        {
            SignalAndWait();
        }
    }

    void DX12CommandHelper::ExecuteCommandListAndWait()
    {
        LD_PROFILE_RENDERER_FUNCTION();

        HRESULT hr = m_CommandList->Close();
        CHECK_DX12_HRESULT(hr, "Failed to close Command List");

        ID3D12CommandList* lists[] = { m_CommandList };
        m_CommandQueue->ExecuteCommandLists(1, lists);
        SignalAndWait();
    }

    void DX12CommandHelper::SignalAndWait()
    {
        LD_PROFILE_RENDERER_FUNCTION();

        m_FenceValue++;
        m_CommandQueue->Signal(m_Fence, m_FenceValue);

        HRESULT hr = m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent);
        CHECK_DX12_HRESULT(hr, "Failed to set Event for Fence");

        auto result = WaitForSingleObject(m_FenceEvent, 2000);
        LD_CORE_ASSERT(result == WAIT_OBJECT_0, "Command Queue took more than 2 seconds to finish execution");
    }

}