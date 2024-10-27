#include "ldpch.h"
#include "DirectX11Context.h"

#include "Ludo/Log.h"
#include "Platform/Windows/WindowsWindow.h"

#include "DirectX11Renderer.h"
#include "DirectX11Utils.h"

namespace Ludo {

	DirectX11Context::DirectX11Context(HWND window)
		: m_Window(window)
	{
	}

	bool DirectX11Context::Init()
	{
		HRESULT hr = S_OK;

		auto Device = DirectX11Renderer::Get()->GetDevice();
		auto DeviceContext = DirectX11Renderer::Get()->GetDeviceContext();

		IDXGIFactory2* Factory;
		hr = CreateDXGIFactory(IID_PPV_ARGS(&Factory));
		if (FAILED(hr))
		{
			LD_CORE_ERROR("Failed to create DXGI Factory");
			ShutDown();
			return false;
		}

		DXGI_SWAP_CHAIN_DESC1 swapdesc = {};
		swapdesc.Width = 1280;
		swapdesc.Height = 720;
		swapdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapdesc.BufferCount = 2;
		swapdesc.SampleDesc.Count = 1;

		Factory->CreateSwapChainForHwnd(
			Device,
			m_Window,
			&swapdesc,
			NULL,
			NULL,
			&m_SwapChain
		);

		CreateRenderTarget();

		// Creating the viewport
		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = 1280;
		viewport.Height = 720;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		DeviceContext->RSSetViewports(1, &viewport);

		return true;
	}

	DirectX11Context::~DirectX11Context()
	{
		ShutDown();
	}

	void DirectX11Context::SwapBuffers()
	{
		auto DeviceContext = DirectX11Renderer::Get()->GetDeviceContext();
		DeviceContext->OMSetRenderTargets(1, &m_BackBuffer, nullptr);
		m_SwapChain->Present(0, 0);
	}

	void DirectX11Context::Clear()
	{
		auto DeviceContext = DirectX11Renderer::Get()->GetDeviceContext();
		static float rgba[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		DeviceContext->ClearRenderTargetView(m_BackBuffer, rgba);
	}

	void DirectX11Context::Resize(unsigned int width, unsigned int height)
	{
		CHECK_AND_RELEASE(m_BackBuffer);
		m_SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
		CreateRenderTarget();
	}

	void DirectX11Context::ShutDown()
	{
		LD_CORE_INFO("Releasing Render context");
		CHECK_AND_RELEASE(m_SwapChain);
		CHECK_AND_RELEASE(m_BackBuffer);
	}

	void DirectX11Context::CreateRenderTarget()
	{
		auto Device = DirectX11Renderer::Get()->GetDevice();
		auto DeviceContext = DirectX11Renderer::Get()->GetDeviceContext();

		ID3D11Texture2D* BackBuffer;
		m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&BackBuffer));

		Device->CreateRenderTargetView(BackBuffer, NULL, &m_BackBuffer);
		BackBuffer->Release();

		DeviceContext->OMSetRenderTargets(1, &m_BackBuffer, NULL);
	}

}