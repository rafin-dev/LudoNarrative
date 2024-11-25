#include "ldpch.h"
#include "DirectX12API.h"

#include "Ludo/Log.h"
#include "Utils/DX12Utils.h"
#include "DirectX12Context.h"
#include "DirectX12Shader.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_dx12.h"
#include "imgui/backends/imgui_impl_win32.h"


namespace Ludo {

    bool DirectX12API::Init()
    {
        HRESULT hr = S_OK;

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
        if (!DirectX::XMVerifyCPUSupport())
        {
            LD_CORE_ERROR("CPU does no support DirectXMath required instructions!");
            ShutDown();
            return false;
        }

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

        LD_CORE_INFO("Initialized DirectX12(D3D12) Render System");

        // ImGui
        D3D12_DESCRIPTOR_HEAP_DESC imguiSrvDesHeapDescription = {};
        imguiSrvDesHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        imguiSrvDesHeapDescription.NumDescriptors = 1;
        imguiSrvDesHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        hr = m_Device->CreateDescriptorHeap(&imguiSrvDesHeapDescription, IID_PPV_ARGS(&m_ImGuiSrvDescHeap));
        VALIDATE_DXCALL_SUCCESS(hr, "Failed to create ImGui SRV Descriptor Heap");

        if (!DirectX12Shader::InitSystem())
        {
            ShutDown();
            return false;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Dockind
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
        LD_CORE_INFO("Initalized ImGui");

        ImGui_ImplDX12_Init(m_Device, DirectX12Context::GetSwapChainBufferCount(),
            DXGI_FORMAT_R8G8B8A8_UNORM, m_ImGuiSrvDescHeap,
            m_ImGuiSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
            m_ImGuiSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

        return true;
    }

    DirectX12API::~DirectX12API()
    {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        ShutDown();
    }

    void DirectX12API::DrawIndexed(const std::shared_ptr<VertexBuffer>& vertexBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer)
    {
        m_CommandList->DrawIndexedInstanced(indexBuffer->GetCount(), 1, 0, 0, 0);
    }

    void DirectX12API::BeginImGui()
    {
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void DirectX12API::EndImGui()
    {
        ImGui::Render();

        m_CommandList->SetDescriptorHeaps(1, &m_ImGuiSrvDescHeap);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_CommandList);

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault(nullptr, (void*)m_CommandList);
        }
    }

    void DirectX12API::ShutDown()
    {
        DirectX12Shader::CloseSystem();
        CHECK_AND_RELEASE_COMPTR(m_ImGuiSrvDescHeap);
        LD_CORE_INFO("Closed ImGui");

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
        LD_CORE_INFO("Closed DirectX12(D3D12) Render system");

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

    void DirectX12API::SignalAndWait()
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

    void DirectX12API::ExecuteCommandListAndWait()
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

    DirectX12API* DirectX12API::Get()
    {
        static DirectX12API system;
        return &system;
    }

}