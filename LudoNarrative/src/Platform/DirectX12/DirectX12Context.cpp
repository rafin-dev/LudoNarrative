#include "ldpch.h"
#include "DirectX12Context.h"

#include "Platform/DirectX12/Utils/DX12Utils.h"
#include "Platform/DirectX12/DirectX12API.h"
#include "Platform/DirectX12/DirectX12FrameBuffer.h"

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
        swapChainDesc.BufferCount = (UINT)GetSwapChainBufferCount();
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
        descRTVheap.NumDescriptors = (UINT)GetSwapChainBufferCount();
        descRTVheap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        descRTVheap.NodeMask = 0;

        hr = device->CreateDescriptorHeap(&descRTVheap, IID_PPV_ARGS(&m_rtvDescriptorHeap));
        VALIDATE_DX12_HRESULT(hr, "Failed to create Render Target View Descriptor Heap");

        // ========== Render Target View CPU Descriptor handle ==========
        auto firstHandle = m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        auto CPUhandleIncrement = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        for (uint32_t i = 0; i < GetSwapChainBufferCount(); i++)
        {
            m_rtvCPUhandles[i] = firstHandle;
            m_rtvCPUhandles[i].ptr += CPUhandleIncrement * i;
        }
        
        // ========== Depth Stencil Descriptor Heap ==========
        D3D12_DESCRIPTOR_HEAP_DESC depthStencilDescHeapDesc = {};
        depthStencilDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        depthStencilDescHeapDesc.NumDescriptors = 1;
        depthStencilDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        depthStencilDescHeapDesc.NodeMask = 0;

        hr = device->CreateDescriptorHeap(&depthStencilDescHeapDesc, IID_PPV_ARGS(&m_DepthStencilDescriptorHeap));
        VALIDATE_DX12_HRESULT(hr, "Failed to create Descriptor Heap for the Depth Stencil View");

        // ========== Miscelanious ==========
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
        CHECK_AND_RELEASE_COMPTR(m_DepthStencilDescriptorHeap);
        CHECK_AND_RELEASE_COMPTR(m_rtvDescriptorHeap);
        CHECK_AND_RELEASE_COMPTR(m_SwapChain);

        LD_CORE_INFO("Destroyed DirectX12 Context for the window: {0} [{1}, {2}]", m_Window->GetTitle(), m_Window->GetWidth(), m_Window->GetHeight());
    }

    inline bool DirectX12Context::RetrieveBuffers()
    {
        LD_PROFILE_FUNCTION();

        for (UINT i = 0; i < GetSwapChainBufferCount(); i++)
        {
            HRESULT hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_Buffers[i]));
            CHECK_DX12_HRESULT(hr, "Failed to retrieve D3D12 Buffer")
            {
                return false;
            }

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

        CHECK_AND_RELEASE_COMPTR(m_DepthStencilBuffer);
        D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
        depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
        depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

        D3D12_CLEAR_VALUE depthStencilClearValue = {};
        depthStencilClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        depthStencilClearValue.DepthStencil.Depth = 1.0f;
        depthStencilClearValue.DepthStencil.Stencil = 0;

        D3D12_HEAP_PROPERTIES heapProperties = {};
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProperties.CreationNodeMask = 0;
        heapProperties.VisibleNodeMask = 0;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resourceDesc.Width = m_Window->GetWidth();
        resourceDesc.Height = m_Window->GetHeight();
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        HRESULT hr = DirectX12API::Get()->GetDevice()->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthStencilClearValue,
            IID_PPV_ARGS(&m_DepthStencilBuffer)
        );
        CHECK_DX12_HRESULT(hr, "Failed to create Depth Stencil Buffer of size: [{0}, {1}]", m_Window->GetWidth(), m_Window->GetHeight())
        {
            return false;
        }

        DirectX12API::Get()->GetDevice()->CreateDepthStencilView(m_DepthStencilBuffer, &depthStencilViewDesc, m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

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

        CHECK_AND_RELEASE_COMPTR(m_DepthStencilBuffer);
    }

    void DirectX12Context::SwapBuffers()
    {
        LD_PROFILE_FUNCTION();

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
        D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle = m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

        commandList->ClearRenderTargetView(m_rtvCPUhandles[m_CurrentBackBuffer], (float*)&RenderCommand::GetClearColor(), 0, nullptr);
        commandList->ClearDepthStencilView(depthStencilViewHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        s_CurrentBAckBufferRTVHandle = m_rtvCPUhandles[m_CurrentBackBuffer];
        s_DepthStencilHandle = m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        commandList->OMSetRenderTargets(1, &m_rtvCPUhandles[m_CurrentBackBuffer], false, &depthStencilViewHandle);
        DirectX12FrameBuffer::s_CurrentBoundFormats =
        {
            DXGI_FORMAT_R8G8B8A8_UNORM,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_D32_FLOAT
        };

        // Viewport
        uint32_t width = m_Window->GetWidth(), height = m_Window->GetHeight();

        D3D12_VIEWPORT viewport = {};
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = (FLOAT)width;
        viewport.Height = (FLOAT)height;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        s_ViewportWidth = width;
        s_ViewportHeight = height;

        // Scissor rectangle
        RECT scissorRect = {};
        scissorRect.left = 0;
        scissorRect.top = 0;
        scissorRect.right = (LONG)viewport.Width;
        scissorRect.bottom = (LONG)viewport.Height;

        // Rasterizer
        commandList->RSSetViewports(1, &viewport);
        commandList->RSSetScissorRects(1, &scissorRect);

        // Texture Descriptor Table/Heap
        DirectX12API::Get()->GetSRVDescriptorHeap().NewFrame();
    }

    inline void DirectX12Context::EndFrame()
    {
        LD_PROFILE_FUNCTION();

        auto& commandList = DirectX12API::Get()->GetCommandList();

        D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle = m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        commandList->OMSetRenderTargets(1, &m_rtvCPUhandles[m_CurrentBackBuffer], false, &depthStencilViewHandle);

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = m_Buffers[m_CurrentBackBuffer];
        barrier.Transition.Subresource = 0;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

        commandList->ResourceBarrier(1, &barrier);

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
        HRESULT hr = m_SwapChain->ResizeBuffers((UINT)GetSwapChainBufferCount(), m_Nwidth, m_Nheight, DXGI_FORMAT_UNKNOWN,
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
        CHECK_DX12_HRESULT(hr, "Failed to Resize DirectX12 Swap Chain for the window: {0} [{1}, {2}]", m_Window->GetTitle(), m_Window->GetWidth(), m_Window->GetHeight());
        RetrieveBuffers();

        LD_CORE_TRACE("Resize DirectX12 Swap Chain for the window: {0} [{1}, {2}]", m_Window->GetTitle(), m_Window->GetWidth(), m_Window->GetHeight());
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Context::s_CurrentBAckBufferRTVHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Context::s_DepthStencilHandle;
    uint32_t DirectX12Context::s_ViewportWidth;
    uint32_t DirectX12Context::s_ViewportHeight;


    void DirectX12Context::SetSwapChainRenderTarget()
    {
        LD_PROFILE_RENDERER_FUNCTION();

        auto& commandList = DirectX12API::Get()->GetCommandList();

        D3D12_VIEWPORT viewport = {};
        viewport.Width = (FLOAT)s_ViewportWidth;
        viewport.Height = (FLOAT)s_ViewportHeight;
        viewport.MaxDepth = 1.0f;

        commandList->RSSetViewports(1, &viewport);
        DirectX12API::Get()->GetCommandList()->OMSetRenderTargets(1, &s_CurrentBAckBufferRTVHandle, false, &s_DepthStencilHandle);
        DirectX12FrameBuffer::s_CurrentBoundFormats =
        {
            DXGI_FORMAT_R8G8B8A8_UNORM,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_UNKNOWN,
            DXGI_FORMAT_D32_FLOAT
        };
    }

}