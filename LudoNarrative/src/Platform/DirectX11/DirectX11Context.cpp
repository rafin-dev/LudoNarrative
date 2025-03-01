#include "ldpch.h"
#include "DirectX11Context.h"

#include "DirectX11API.h"
#include "DirectX11FrameBuffer.h"
#include "Utils/DirectX11Utils.h"

namespace Ludo {

    DirectX11Context::DirectX11Context(HWND window)
        : m_WindowHandle(window)
    {
        m_Window = (WindowsWindow*)GetWindowLongPtr(m_WindowHandle, GWLP_USERDATA);
    }

    DirectX11Context::~DirectX11Context()
    {
        ShutDown();
    }

    bool DirectX11Context::Init()
    {
        LD_PROFILE_FUNCTION();

        HRESULT hr = S_OK;
        auto* api = DirectX11API::Get();

        // =========== Swap Chain ===========
        DXGI_SWAP_CHAIN_DESC1 swapDesc = {};
        swapDesc.BufferCount = GetSwapChainBufferCount();
        swapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapDesc.Width = m_Window->GetWidth();
        swapDesc.Height = m_Window->GetHeight();
        swapDesc.Scaling = DXGI_SCALING_STRETCH;
        swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        swapDesc.SampleDesc.Count = 1;
        swapDesc.SampleDesc.Quality = 0;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC scfd = {};
        scfd.Windowed = true;

        IDXGISwapChain1* swapchain1 = nullptr;
        hr = api->GetFactory()->CreateSwapChainForHwnd(api->GetDevice(), m_WindowHandle, &swapDesc, &scfd, nullptr, &swapchain1);
        VALIDATE_DX_HRESULT(hr, "Failed to create DXGI Swap Chain for window of size: [{0}, {1}]", m_Window->GetWidth(), m_Window->GetHeight());
        hr = swapchain1->QueryInterface(&m_SwapChain);
        VALIDATE_DX_HRESULT(hr, "Failed to query IDXGISwapChain2 from IDXGISwapChain1");
        swapchain1->Release();

        // ========== Depth Buffer ==========
        if (!CreateDepthStencilView())
        {
            return false;
        }

        D3D11_DEPTH_STENCIL_DESC depthStateDesc = {};
        depthStateDesc.DepthEnable = true;
        depthStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

        depthStateDesc.StencilEnable = false;
        depthStateDesc.StencilReadMask = 0xFF;
        depthStateDesc.StencilWriteMask = 0xFF;

        depthStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        depthStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        depthStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        depthStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        hr = api->GetDevice()->CreateDepthStencilState(&depthStateDesc, &m_DepthStencilState);
        VALIDATE_DX_HRESULT(hr, "Failed to create Depth Stencil State");

        // ========== Blend State ==========
        D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
        rtBlendDesc.BlendEnable = true;
        rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
        rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
        rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
        rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
        rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        D3D11_BLEND_DESC BlendDesc = {};
        BlendDesc.AlphaToCoverageEnable = false;
        BlendDesc.IndependentBlendEnable = false;
        BlendDesc.RenderTarget[0] = rtBlendDesc;

        hr = api->GetDevice()->CreateBlendState(&BlendDesc, &m_BlendState);

        // ========== Back Buffer / Render Target ==========
        if (!GetBackBuffer())
        {
            return false;
        }

        // ========== ViewPort ==========
        SetViewPort();

        hr = api->GetFactory()->MakeWindowAssociation(m_WindowHandle, DXGI_MWA_NO_ALT_ENTER);

        auto color = api->GetClearColor();
        api->GetDeviceContext()->ClearRenderTargetView(m_BackBuffer, (float*)&color);

        LD_CORE_INFO("Created DirectX11 Render Context for Window: {0} [{1}, {2}]", m_Window->GetTitle(), m_Window->GetWidth(), m_Window->GetHeight());

        return true;
    }

    void DirectX11Context::SwapBuffers()
    {
        LD_PROFILE_FUNCTION();

        auto deviceContext = DirectX11API::Get()->GetDeviceContext();

        // End Last frame
        m_SwapChain->Present(m_Window->IsVsync(), m_Window->IsVsync() ? 0 : DXGI_PRESENT_ALLOW_TEARING);

        if (m_ShouldResize)
        {
            m_ShouldResize = false;
            ResizeImpl();
        }

        // Begin new frame
        deviceContext->OMSetRenderTargets(1, &m_BackBuffer, m_DepthStencilView);
        deviceContext->OMSetDepthStencilState(m_DepthStencilState, 1);
        deviceContext->ClearRenderTargetView(m_BackBuffer, (float*)&DirectX11API::Get()->GetClearColor());
        deviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        deviceContext->OMSetBlendState(m_BlendState, nullptr, 0xffffffff);
    }

    void DirectX11Context::Resize(unsigned int width, unsigned int height)
    {
        m_ShouldResize = true;
    }

    void DirectX11Context::ShutDown()
    {
        LD_PROFILE_FUNCTION();

        CHECK_AND_RELEASE_COMPTR(m_DepthStencilView);
        CHECK_AND_RELEASE_COMPTR(m_DepthStencilState);
        CHECK_AND_RELEASE_COMPTR(m_BackBuffer);
        CHECK_AND_RELEASE_COMPTR(m_SwapChain);
        CHECK_AND_RELEASE_COMPTR(m_BlendState);
        LD_CORE_INFO("Deleted DirectX11 Render Context for window: {0}", m_Window->GetTitle());
    }

    bool DirectX11Context::GetBackBuffer()
    {
        CHECK_AND_RELEASE_COMPTR(m_BackBuffer);
        ID3D11Texture2D* pBackBuffer = nullptr;
        HRESULT hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        VALIDATE_DX_HRESULT(hr, "Failed to retrieve Back Buffer from Swap Chain");

        hr = DirectX11API::Get()->GetDevice()->CreateRenderTargetView(pBackBuffer, nullptr, &m_BackBuffer);
        pBackBuffer->Release();
        VALIDATE_DX_HRESULT(hr, "Failed to create Render Target View for Back Buffer");

        DirectX11API::Get()->GetDeviceContext()->OMSetRenderTargets(1, &m_BackBuffer, m_DepthStencilView);
        DirectX11API::Get()->GetDeviceContext()->OMSetDepthStencilState(m_DepthStencilState, 1);

        DirectX11FrameBuffer::SwapChainTarget = m_BackBuffer;
        DirectX11FrameBuffer::SwapChainDepthStencil = m_DepthStencilView;
        DirectX11FrameBuffer::SwapChainBlendSate = m_BlendState;

        return true;
    }

    bool DirectX11Context::CreateDepthStencilView()
    {
        CHECK_AND_RELEASE_COMPTR(m_DepthStencilView);

        D3D11_TEXTURE2D_DESC depthTextureDesc = {};
        depthTextureDesc.Width = m_Window->GetWidth();
        depthTextureDesc.Height = m_Window->GetHeight();
        depthTextureDesc.MipLevels = 1;
        depthTextureDesc.ArraySize = 1;
        depthTextureDesc.SampleDesc.Count = 1;
        depthTextureDesc.SampleDesc.Quality = 0;
        depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
        depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        ID3D11Texture2D* DepthStencilTexture;
        HRESULT hr = DirectX11API::Get()->GetDevice()->CreateTexture2D(&depthTextureDesc, NULL, &DepthStencilTexture);
        VALIDATE_DX_HRESULT(hr, "Failed to create Texture2d for Depth Buffer");

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

        hr = DirectX11API::Get()->GetDevice()->CreateDepthStencilView(DepthStencilTexture, &dsvDesc, &m_DepthStencilView);
        DepthStencilTexture->Release();
        VALIDATE_DX_HRESULT(hr, "Failed to create Depth Stencil View");

        return true;
    }

    void DirectX11Context::ResizeImpl()
    {
        CHECK_AND_RELEASE_COMPTR(m_BackBuffer);
        HRESULT hr = m_SwapChain->ResizeBuffers(GetSwapChainBufferCount(), m_Window->GetWidth(), m_Window->GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
        LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to resize Swap Chain to size: [{0}, {1}]", m_Window->GetWidth(), m_Window->GetHeight());

        CreateDepthStencilView();
        GetBackBuffer();
        SetViewPort();
        LD_CORE_INFO("Resized D3D11 Swap Chain for Window: {0}", m_Window->GetTitle());
    }

    void DirectX11Context::SetViewPort()
    {
        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = m_Window->GetWidth();
        viewport.Height = m_Window->GetHeight();
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        DirectX11FrameBuffer::SwapChainViewport = viewport;

        DirectX11API::Get()->GetDeviceContext()->RSSetViewports(1, &viewport);
    }

}