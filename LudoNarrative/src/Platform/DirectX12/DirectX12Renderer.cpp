#include "ldpch.h"
#include "DirectX12Renderer.h"

#include "Ludo/Log.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_dx12.h"

#include "DX12Utils.h"

namespace Ludo {

    bool DirectX12Renderer::Init()
    {
        HRESULT hr = S_OK;

        LD_CORE_INFO("Intializing DirectX12(D3D12) Render System");
#ifdef LUDO_DEBUG
        // Initialize Debug Layer
        // D3D12 Debug
        hr = D3D12GetDebugInterface(IID_PPV_ARGS(&m_D3D12Debug));
        VALIDATE_DXCALL_SUCCESS(hr, "Failed to Retrieve D3D12 Debug Inteface");
        m_D3D12Debug->EnableDebugLayer();

        // DXGI Debug
        hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_DXGIDebug));
        VALIDATE_DXCALL_SUCCESS(hr, "Failed to retrieve DXGI Debug Interface");
        m_DXGIDebug->EnableLeakTrackingForThread();
#endif 
        // DXGI Factory
        hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_DXGIFactory));
        VALIDATE_DXCALL_SUCCESS(hr, "Failed to create DXGI FActory");

        // D3D12 Device
        hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));
        VALIDATE_DXCALL_SUCCESS(hr, "Failed to create D3D12 Device");

        // D3D12 CommandQueue
        D3D12_COMMAND_QUEUE_DESC cmdDesc = {};
        cmdDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        cmdDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        cmdDesc.NodeMask = 0;
        cmdDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        hr = m_Device->CreateCommandQueue(&cmdDesc, IID_PPV_ARGS(&m_CommandQueue));
        VALIDATE_DXCALL_SUCCESS(hr, "Failed to create the D3D12 CommandQueue");

        // D3D12 Fence
        hr = m_Device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
        VALIDATE_DXCALL_SUCCESS(hr, "Failed to create D3D12 Fence");

        // Fence Event
        m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);
        hr = m_FenceEvent != nullptr ? S_OK : E_FAIL;
        VALIDATE_DXCALL_SUCCESS(hr, "Failed to create Fence Event");

        // D3D12 CommandAllocator
        hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));
        VALIDATE_DXCALL_SUCCESS(hr, "Failed to create D3D12 Command Allocator");

        // D3D12 GraphicsCommandList
        hr = m_Device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_CommandList));
        VALIDATE_DXCALL_SUCCESS(hr, "Failed to create D3D12 Graphics Command List");

        return true;
    }

    DirectX12Renderer::~DirectX12Renderer()
    {
        ShutDown();
    }

    void DirectX12Renderer::BeginImGui()
    {
    }

    void DirectX12Renderer::EndImGui()
    {
    }

    void DirectX12Renderer::ShutDown()
    {
        LD_CORE_INFO("Closing DirectX12(D3D12) Render system");

        CHECK_AND_RELEASE_COMPTR(m_Device);
        CHECK_AND_RELEASE_COMPTR(m_DXGIFactory);

        CHECK_AND_RELEASE_COMPTR(m_CommandQueue);
        CHECK_AND_RELEASE_COMPTR(m_CommandList);
        CHECK_AND_RELEASE_COMPTR(m_CommandAllocator);

        CHECK_AND_RELEASE_COMPTR(m_Fence);
        if (m_FenceEvent != nullptr)
        {
            CloseHandle(m_FenceEvent);
        }

#ifdef LUDO_DEBUG
        if (m_DXGIDebug != nullptr)
        {
            OutputDebugString(L"DXGI Reports living device objects:\n");
            m_DXGIDebug->ReportLiveObjects(
                DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
            );
        }
        CHECK_AND_RELEASE_COMPTR(m_D3D12Debug);
        CHECK_AND_RELEASE_COMPTR(m_DXGIDebug);
#endif 
    }

    void DirectX12Renderer::SignalAndWait()
    {
        m_FenceValue++;
        m_CommandQueue->Signal(m_Fence, m_FenceValue);
        if (SUCCEEDED(m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent)))
        {
            if (WaitForSingleObject(m_FenceEvent, 20000) != WAIT_OBJECT_0)
            {
                std::exit(-1);
            }
        }
        else
        {
            std::exit(-1);
        }
    }

    void DirectX12Renderer::ExecuteCommandList()
    {
        if (FAILED(m_CommandList->Close()))
        {
            // TODO: Implement crahsing system
            std::exit(-1);
        }

        ID3D12CommandList* lists[] = { m_CommandList };
        m_CommandQueue->ExecuteCommandLists(1, lists);
        SignalAndWait();
    }

    InternalRenderer* InternalRenderer::Get()
    {
        static DirectX12Renderer instance;
        return &instance;
    }
}