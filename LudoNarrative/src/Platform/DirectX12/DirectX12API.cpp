#include "ldpch.h"
#include "DirectX12API.h"

#include "Platform/DirectX12/Utils/DX12Utils.h"
#include "Platform/DirectX12/Utils/DX12ShaderCompiler.h"
#include "Platform/DirectX12/DirectX12Context.h"
#include "Platform/DirectX12/DirectX12Shader.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_dx12.h"
#include "imgui/backends/imgui_impl_win32.h"

namespace Ludo {

    bool DirectX12API::Init()
    {
        LD_PROFILE_FUNCTION();

        HRESULT hr = S_OK;

#ifdef LUDO_DEBUG
        // Initialize Debug Layer
        // D3D12 Debug
        hr = D3D12GetDebugInterface(IID_PPV_ARGS(&m_D3D12Debug));
        VALIDATE_DX12_HRESULT(hr, "Failed to Retrieve D3D12 Debug Inteface");
        m_D3D12Debug->EnableDebugLayer();

        // DXGI Debug
        hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_DXGIDebug));
        VALIDATE_DX12_HRESULT(hr, "Failed to retrieve DXGI Debug Interface");
        m_DXGIDebug->EnableLeakTrackingForThread();
#endif 

        if (!DirectX::XMVerifyCPUSupport())
        {
            LD_CORE_ASSERT(false, "CPU does not support DirectXMath required instructions!");
            ShutDown();
            return false;
        }

        // DXGI Factory
        hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_DXGIFactory));
        VALIDATE_DX12_HRESULT(hr, "Failed to create DXGI FActory");

        hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device));
        VALIDATE_DX12_HRESULT(hr, "Failed to create D3D12 Device");

        if (!m_GraphicsCommands.Init()) { return false; }
        if (!m_CopyCommands.Init()) { return false; }

        if (!DX12ShaderCompiler::Init())
        {
            ShutDown();
            return false;
        }

        if (!DirectX12Shader::CreateRootSignature())
        {
            ShutDown();
            return false;
        }

        if (!m_SrvDescriptorHeap.Init())
        {
            ShutDown();
            return false;
        }

        LD_CORE_INFO("Initialized DirectX12(D3D12) Render API");

        if (!InitImGui())
        {
            return false;
        }

        return true;
    }

    static DX12SRVDescriptorHeap* s_DescriptorHeapLambdaAcces;

    bool DirectX12API::InitImGui()
    {
        LD_PROFILE_FUNCTION();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
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

        s_DescriptorHeapLambdaAcces = &m_SrvDescriptorHeap;

        ImGui_ImplDX12_InitInfo initInfo = {};
        initInfo.Device = m_Device;
        initInfo.CommandQueue = m_GraphicsCommands.GetCommandQueue();
        initInfo.NumFramesInFlight = DirectX12Context::GetSwapChainBufferCount();
        initInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        initInfo.SrvDescriptorHeap = m_SrvDescriptorHeap.GetDescriptorHeap();
        initInfo.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
            {
                return s_DescriptorHeapLambdaAcces->Alloc(out_cpu_handle, out_gpu_handle);
            };
        initInfo.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
            {
                return s_DescriptorHeapLambdaAcces->Free(cpu_handle, gpu_handle);
            };
        if (!ImGui_ImplDX12_Init(&initInfo))
        {
            return false;
        }

        m_ImGuiInitialized = true;

        LD_CORE_INFO("Initialized ImGui");

        return true;
    }

    void DirectX12API::CloseImGui()
    {
        LD_PROFILE_FUNCTION();

        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        LD_CORE_INFO("Closed ImGui");
    }

    void DirectX12API::ShutDown()
    {
        LD_PROFILE_FUNCTION();
        
        if (m_ImGuiInitialized)
        {
            CloseImGui();
            m_ImGuiInitialized = false;
        }

        DX12ShaderCompiler::ShutDown();
        DirectX12Shader::DestroyRootSignature();

        m_GraphicsCommands.ShutDown();
        m_CopyCommands.ShutDown();

        m_SrvDescriptorHeap.ShutDown();

        CHECK_AND_RELEASE_COMPTR(m_Device);
        CHECK_AND_RELEASE_COMPTR(m_DXGIFactory);

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
#endif // LUDO_DEBUG

        LD_CORE_INFO("Closed DirectX12(D3D12) Render API");
    }

    void DirectX12API::SetClearColor(const DirectX::XMFLOAT4& color)
    {
        m_ClearColor = color;
    }

    const DirectX::XMFLOAT4& DirectX12API::GetClearColor()
    {
        return m_ClearColor;
    }

    void DirectX12API::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
    {
        LD_PROFILE_FUNCTION();

        auto& commandList = m_GraphicsCommands.GetCommandList();

        m_SrvDescriptorHeap.BindToDescriptorTable();
        commandList->DrawIndexedInstanced(indexCount == 0 ? vertexArray->GetIndexBuffer()->GetCount() : indexCount, 1, 0, 0, 0);
    }

    void DirectX12API::BeginImGui()
    {
        LD_PROFILE_FUNCTION();

        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void DirectX12API::EndImGui()
    {
        LD_PROFILE_FUNCTION();

        ImGui::Render();

        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_GraphicsCommands.GetCommandList());

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault(nullptr, (void*)m_GraphicsCommands.GetCommandList());
        }
    }

    DirectX12API* DirectX12API::Get()
    {
        static DirectX12API instance;
        return &instance;
    }

    void DirectX12API::ImediateCopyBufferRegion(ID3D12Resource2* dest, size_t destOffset, ID3D12Resource2* src, size_t srcOffset, size_t size)
    {
        auto commandList = m_CopyCommands.InitCommandList();

        commandList->CopyBufferRegion(dest, destOffset, src, srcOffset, size);

        m_CopyCommands.ExecuteCommandListAndWait();
    }

    void DirectX12API::ImediateCopyTextureRegion(const D3D12_TEXTURE_COPY_LOCATION& dest, uint32_t destX, uint32_t destY, uint32_t destZ, const D3D12_TEXTURE_COPY_LOCATION& src, const D3D12_BOX& srcBox)
    {
        auto commandList = m_CopyCommands.InitCommandList();

        commandList->CopyTextureRegion(&dest, destX, destY, destZ, &src, &srcBox);

        m_CopyCommands.ExecuteCommandListAndWait();

    }

}