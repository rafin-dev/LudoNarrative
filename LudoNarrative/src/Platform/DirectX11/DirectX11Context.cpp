#include "ldpch.h"
#include "DirectX11Context.h"

#include "DirectX11API.h"
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

        hr = api->GetFactory()->CreateSwapChainForHwnd(api->GetDevice(), m_WindowHandle, &swapDesc, &scfd, nullptr, &m_SwapChain);
        VALIDATE_DX_HRESULT(hr, "Failed to create DXGI Swap Chain for window of size: [{0}, {1}]", m_Window->GetWidth(), m_Window->GetHeight());

        // ========== Back Buffer / Render Target ==========
        if (!GetBackBuffer())
        {
            return false;
        }

        // ========== ViewPort ==========
        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = m_Window->GetWidth();
        viewport.Height = m_Window->GetHeight();

        api->GetDeviceContext()->RSSetViewports(1, &viewport);

        api->GetFactory()->MakeWindowAssociation(m_WindowHandle, DXGI_MWA_NO_ALT_ENTER);

        auto color = api->GetClearColor();
        api->GetDeviceContext()->ClearRenderTargetView(m_BackBuffer, (float*)&color);

        LD_CORE_INFO("Created DirectX11 Render Context for Window: {0} [{1}, {2}]", m_Window->GetTitle(), m_Window->GetWidth(), m_Window->GetHeight());

        return true;
    }

    void DirectX11Context::SwapBuffers()
    {
        auto deviceContext = DirectX11API::Get()->GetDeviceContext();
        
        // End Last frame
        m_SwapChain->Present(0, 0);
        DirectX11API::Get()->GetDeviceContext()->OMSetRenderTargets(1, &m_BackBuffer, nullptr);

        if (m_ShouldResize)
        {
            ResizeImpl();
        }

        // Begin new
        auto color = DirectX11API::Get()->GetClearColor();
        deviceContext->ClearRenderTargetView(m_BackBuffer, (float*)&color);
    }

    void DirectX11Context::Resize(unsigned int width, unsigned int height)
    {
        m_ShouldResize = true;
    }

    void DirectX11Context::ShutDown()
    {
        CHECK_AND_RELEASE_COMPTR(m_BackBuffer);
        CHECK_AND_RELEASE_COMPTR(m_SwapChain);
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

        DirectX11API::Get()->GetDeviceContext()->OMSetRenderTargets(1, &m_BackBuffer, nullptr);

        return true;
    }

    void DirectX11Context::ResizeImpl()
    {
        m_ShouldResize = false;

        HRESULT hr = m_SwapChain->ResizeBuffers(1, m_Window->GetWidth(), m_Window->GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
        LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to resize Swap Chain to size: [{0}, {1}]", m_Window->GetWidth(), m_Window->GetHeight());

        GetBackBuffer();
    }

}