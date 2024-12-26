#include "ldpch.h"
#include "DirectX12Context.h"

#include "Platform/DirectX12/Utils/DX12Utils.h"
#include "Platform/DirectX12/DirectX12API.h"

#include "Ludo/Renderer/RenderCommand.h"

namespace Ludo {

    DirectX12Context::DirectX12Context(HWND window)
        : m_WindowHandle(window)
    {
    }

    bool DirectX12Context::Init()
    {
        LD_PROFILE_FUNCTION();
        
        HRESULT hr = S_OK;
        auto& factory = DirectX12API::Get()->GetDXGIFactory();
        auto& device = DirectX12API::Get()->GetDevice();
        m_Window = (WindowsWindow*)GetWindowLongPtr(m_WindowHandle, GWLP_USERDATA);
        RECT size = {};
        GetClientRect(m_WindowHandle, &size);

        // ========== Swap Chain ===========
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = size.right - size.left;
        swapChainDesc.Height = size.bottom - size.top;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.Stereo = false;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = GetSwapChainBufferCount();
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC scfd = {};
        scfd.Windowed = true;

        IDXGISwapChain1* sc1 = nullptr;
        hr = factory->CreateSwapChainForHwnd(
            DirectX12API::Get()->GetCommandQueue(),
            m_WindowHandle,
            &swapChainDesc,
            &scfd,
            nullptr,
            &sc1
        );
        VALIDATE_DX12_HRESULT(hr, "Failed to Create DXGI Swap Chain");

        hr = sc1->QueryInterface(&m_SwapChain);
        VALIDATE_DX12_HRESULT(hr, "Failed to Query IDXGISwapChain4 interface from IDXGISwapChain1");
        sc1->Release();

        // ========== RTV decriptor heap ==========
        D3D12_DESCRIPTOR_HEAP_DESC descRTVheap = {};
        descRTVheap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        descRTVheap.NumDescriptors = GetSwapChainBufferCount();
        descRTVheap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        descRTVheap.NodeMask = 0;

        hr = device->CreateDescriptorHeap(&descRTVheap, IID_PPV_ARGS(&m_rtvDescriptorHeap));
        VALIDATE_DX12_HRESULT(hr, "Failed to create Render Target View Descriptor Heap");

        // ========== Render Target View CPU Descriptor handle ==========
        auto firstHandle = m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        auto CPUhandleIncrement = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        for (int i = 0; i < GetSwapChainBufferCount(); i++)
        {
            m_rtvCPUhandles[i] = firstHandle;
            m_rtvCPUhandles[i].ptr += CPUhandleIncrement * i;
        }

        if (!RetrieveBuffers())
        {
            return false;
        }

        factory->MakeWindowAssociation(m_WindowHandle, DXGI_MWA_NO_ALT_ENTER);

        LD_CORE_INFO("Created DirectX12(D3D12) Graphics Context: {0}, {1}", m_Window->GetWidth(), m_Window->GetHeight());

        BeginFrame();

        return true;
    }

    void DirectX12Context::ShutDown()
    {
        LD_PROFILE_FUNCTION();
        
        ReleaseBuffers();
        CHECK_AND_RELEASE_COMPTR(m_rtvDescriptorHeap);
        CHECK_AND_RELEASE_COMPTR(m_SwapChain);

        LD_CORE_INFO("Destroyed DirectX12 Context for the window: {0} [{1}, {2}]", m_Window->GetTitle(), m_Window->GetWidth(), m_Window->GetHeight());
    }

    inline bool DirectX12Context::RetrieveBuffers()
    {
        LD_PROFILE_FUNCTION();

        for (size_t i = 0; i < GetSwapChainBufferCount(); i++)
        {
            HRESULT hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_Buffers[i]));
            CHECK_DX12_HRESULT(hr, "Failed to retrieve D3D12 Buffer");

            D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
            rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            rtvDesc.Texture2D.MipSlice = 0;
            rtvDesc.Texture2D.PlaneSlice = 0;

            DirectX12API::Get()->GetDevice()->CreateRenderTargetView(
                m_Buffers[i],
                &rtvDesc,
                m_rtvCPUhandles[i]
            );
        }

        return true;
    }

    inline void DirectX12Context::ReleaseBuffers()
    {
        LD_PROFILE_FUNCTION();

        DirectX12API::Get()->Flush(GetSwapChainBufferCount());

        for (size_t i = 0; i < GetSwapChainBufferCount(); i++)
        {
            CHECK_AND_RELEASE_COMPTR(m_Buffers[i]);
        }
    }

    void DirectX12Context::SwapBuffers()
    {
        EndFrame();

        m_SwapChain->Present(m_Window->IsVsync(), m_Window->IsVsync() ? 0 : DXGI_PRESENT_ALLOW_TEARING);

        if (m_ShouldResize)
        {
            ResizeImpl();
        }

        BeginFrame();
    }

    inline void DirectX12Context::BeginFrame()
    {
        LD_PROFILE_FUNCTION();

        auto& commandList = DirectX12API::Get()->InitCommandList();
        m_CurrentBackBuffer = m_SwapChain->GetCurrentBackBufferIndex();

        // Resource Barrier
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = m_Buffers[m_CurrentBackBuffer];
        barrier.Transition.Subresource = 0;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

        commandList->ResourceBarrier(1, &barrier);

        // Clar and set RenderTarget
        commandList->ClearRenderTargetView(m_rtvCPUhandles[m_CurrentBackBuffer], (float*)&RenderCommand::GetClearColor(), 0, nullptr);

        commandList->OMSetRenderTargets(1, &m_rtvCPUhandles[m_CurrentBackBuffer], false, nullptr);

        // Viewport
        D3D12_VIEWPORT viewport = {};
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = m_Window->GetWidth();
        viewport.Height = m_Window->GetHeight();
        viewport.MinDepth = 1.0f;
        viewport.MaxDepth = 0.0f;

        // Scissor rectangle
        RECT scissorRect = {};
        scissorRect.left = 0;
        scissorRect.top = 0;
        scissorRect.right = viewport.Width;
        scissorRect.bottom = viewport.Height;

        // Rasterizer
        commandList->RSSetViewports(1, &viewport);
        commandList->RSSetScissorRects(1, &scissorRect);
    }

    inline void DirectX12Context::EndFrame()
    {
        LD_PROFILE_FUNCTION();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = m_Buffers[m_CurrentBackBuffer];
        barrier.Transition.Subresource = 0;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

        DirectX12API::Get()->GetCommandList()->ResourceBarrier(1, &barrier);

        DirectX12API::Get()->ExecuteCommandListAndWait();
    }

    DirectX12Context::~DirectX12Context()
    {
        ShutDown();
    }

    void DirectX12Context::Resize(uint32_t width, uint32_t height)
    {
        m_ShouldResize = true;
    }

    void DirectX12Context::ResizeImpl()
    {
        LD_PROFILE_FUNCTION();

        m_ShouldResize = false;
        DirectX12API::Get()->Flush(GetSwapChainBufferCount());

        ReleaseBuffers();
        HRESULT hr = m_SwapChain->ResizeBuffers(GetSwapChainBufferCount(), m_Nwidth, m_Nheight, DXGI_FORMAT_UNKNOWN,
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
        CHECK_DX12_HRESULT(hr, "Failed to Resize DirectX12 Swap Chain for the window: {0} [{1}, {2}]", m_Window->GetTitle(), m_Window->GetWidth(), m_Window->GetHeight());

        LD_CORE_TRACE("Resize DirectX12 Swap Chain for the window: {0} [{1}, {2}]", m_Window->GetTitle(), m_Window->GetWidth(), m_Window->GetHeight());
    }

}