#include "ldpch.h"
#include "DirectX12Context.h"

#include "DirectX12Renderer.h"
#include "DX12Utils.h"

#include "Platform/Windows/WindowsWindow.h"
#include "Ludo/Application.h"

namespace Ludo {

	DirectX12Context::DirectX12Context(HWND window)
		: m_Window(window)
	{

	}

	DirectX12Context::~DirectX12Context()
	{
		ShutDown();
	}

	bool DirectX12Context::Init()
	{
		HRESULT hr = S_OK;
		auto& factory = DirectX12Renderer::Get()->GetDXGIFactory();
		WindowsWindow* window = (WindowsWindow*)GetWindowLongPtr(m_Window, GWLP_USERDATA);
		RECT size = {};
		GetClientRect(m_Window, &size);

		DXGI_SWAP_CHAIN_DESC1 scd = {};
		scd.Width = size.right - size.left;
		scd.Height = size.bottom - size.top;
		scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.Stereo = false;
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
		scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = GetSwapChainBufferCount();
		scd.Scaling = DXGI_SCALING_STRETCH;
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		scd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC scfd = {};
		scfd.Windowed = true;

		IDXGISwapChain1* sc1 = nullptr;

		 hr = factory->CreateSwapChainForHwnd(
			DirectX12Renderer::Get()->GetCommandQueue(),
			m_Window,
			&scd,
			&scfd,
			nullptr,
			&sc1
		);
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to create DXGI Swap Chain");
		hr = sc1->QueryInterface(&m_SwapChain);
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to retrieve DXGI Swap Chain");
		sc1->Release();

		LD_CORE_INFO("Created DirectX12(D3D12) Graphics Context: {0}, {1}", window->GetWidth(), window->GetHeight());

		DirectX12Renderer::Get()->InitCommandList();

		return true;
	}

	void DirectX12Context::Resize(unsigned int width, unsigned int height)
	{
		DirectX12Renderer::Get()->Flush(GetSwapChainBufferCount());
		LD_CORE_TRACE("Resizing D3D12 Graphics Context: {0}, {1}", width, height);
		m_SwapChain->ResizeBuffers(GetSwapChainBufferCount(), width, height, DXGI_FORMAT_UNKNOWN, 
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
	}

	void DirectX12Context::SetFullScreen(bool enabled)
	{

	}	

	void DirectX12Context::SwapBuffers()
	{
		DirectX12Renderer::Get()->ExecuteCommandList();

		m_SwapChain->Present(1, 0);

		// TODO: Clear the back buffer

		DirectX12Renderer::Get()->InitCommandList();
	}

	void DirectX12Context::ShutDown()
	{
		DirectX12Renderer::Get()->Flush(GetSwapChainBufferCount());

		LD_CORE_INFO("Closing DirectX12(D3D12) Graphics Context");
		CHECK_AND_RELEASE_COMPTR(m_SwapChain);
	}

}