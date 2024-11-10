#include "ldpch.h"
#include "DirectX12Context.h"

#include "DirectX12System.h"
#include "DX12Utils.h"

#include "Platform/Windows/WindowsWindow.h"
#include "Ludo/Application.h"
#include "Ludo/Renderer/Shader.h"

namespace Ludo {

	DirectX12Context::DirectX12Context(HWND window)
		: m_WindowHandle(window)
	{

	}

	DirectX12Context::~DirectX12Context()
	{
		EndFrame();
		DirectX12System::Get()->Flush(GetSwapChainBufferCount());

		ShutDown();
	}

	static D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	static D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
	static size_t countIndicies = 0;

	class ImageLoader
	{
	public:
		struct ImageData
		{
			std::vector<char> Data;
			uint32_t Width;
			uint32_t Height;
			uint32_t BitsPerPixel;
			uint32_t ChanelCount;

			GUID PixelFormat;
			DXGI_FORMAT DxgiPixelFormat;
		};

		static bool LoadImageFromDisc(const std::filesystem::path& imagePath, ImageData& data)
		{
			// Factory
			IWICImagingFactory* wicFactory = nullptr;
			HRESULT hr = S_OK;

			hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
			if (FAILED(hr))
			{
				LD_CORE_ERROR("Failed to Initialize COM library");
				return false;
			}
			hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
			if (FAILED(hr))
			{
				LD_CORE_ERROR("Failed to create WIC Imaging Factory");
				return false;
			}

			// Load Image
			IWICStream* wicFileStream = nullptr;
			hr = wicFactory->CreateStream(&wicFileStream);
			if (FAILED(hr))
			{
				LD_CORE_ERROR("Failed to create WIC stream");
				CHECK_AND_RELEASE_COMPTR(wicFactory);
				return false;
			}
			hr = wicFileStream->InitializeFromFilename(imagePath.wstring().c_str(), GENERIC_READ);
			if (FAILED(hr))
			{
				LD_CORE_ERROR("Failed to initialize WIC stream from image: {0}", imagePath.string());
				CHECK_AND_RELEASE_COMPTR(wicFileStream);
				CHECK_AND_RELEASE_COMPTR(wicFactory);
				return false;
			}
			IWICBitmapDecoder* wicBitMapDecoder = nullptr;
			hr = wicFactory->CreateDecoderFromStream(wicFileStream, nullptr, WICDecodeMetadataCacheOnDemand, &wicBitMapDecoder);
			if (FAILED(hr))
			{
				LD_CORE_ERROR("Failed to create WIC BitMap decoder for image: {0}", imagePath.string());
				CHECK_AND_RELEASE_COMPTR(wicFileStream);
				CHECK_AND_RELEASE_COMPTR(wicFactory);
				return false;
			}
			IWICBitmapFrameDecode* wicFrameDecoder = nullptr;
			if (FAILED(wicBitMapDecoder->GetFrame(0, &wicFrameDecoder)))
			{
				LD_CORE_ERROR("Failed to get decode from image: {0}", imagePath.string());
				CHECK_AND_RELEASE_COMPTR(wicFileStream);
				CHECK_AND_RELEASE_COMPTR(wicFactory);
				CHECK_AND_RELEASE_COMPTR(wicBitMapDecoder);
			}

			// Image Metadata
			if (FAILED(wicFrameDecoder->GetSize(&data.Width, &data.Height)))
			{
				LD_CORE_ERROR("Failed to get size from image: {0}");
				CHECK_AND_RELEASE_COMPTR(wicFileStream);
				CHECK_AND_RELEASE_COMPTR(wicFactory);
				CHECK_AND_RELEASE_COMPTR(wicBitMapDecoder);
				CHECK_AND_RELEASE_COMPTR(wicFrameDecoder);
				return false;
			}
			if (FAILED(wicFrameDecoder->GetPixelFormat(&data.PixelFormat)))
			{
				LD_CORE_ERROR("Failed to get pixel format from image: {0}");
				CHECK_AND_RELEASE_COMPTR(wicFileStream);
				CHECK_AND_RELEASE_COMPTR(wicFactory);
				CHECK_AND_RELEASE_COMPTR(wicBitMapDecoder);
				CHECK_AND_RELEASE_COMPTR(wicFrameDecoder);
				return false;
			}

			// Pixel Format Metadata
			IWICComponentInfo* wicComponentInfo = nullptr;
			//if (FAILED(wicFactory->CreateComponentInfo(IID_PPV_ARGS(&wicComponentInfo))))
			//{

			//}

			CHECK_AND_RELEASE_COMPTR(wicFileStream);
			CHECK_AND_RELEASE_COMPTR(wicFactory);
			CHECK_AND_RELEASE_COMPTR(wicBitMapDecoder);
			CHECK_AND_RELEASE_COMPTR(wicFrameDecoder);
			CHECK_AND_RELEASE_COMPTR(wicComponentInfo);
			return true;
		}

	private:
		ImageLoader() = default;
		ImageLoader(const ImageLoader&) = default;
		ImageLoader& operator=(const ImageLoader&) = default;
	};

	bool DirectX12Context::Init()
	{
		HRESULT hr = S_OK;
		auto& factory = DirectX12System::Get()->GetDXGIFactory();
		auto& device = DirectX12System::Get()->GetDevice();
		m_Window = (WindowsWindow*)GetWindowLongPtr(m_WindowHandle, GWLP_USERDATA);
		RECT size = {};
		GetClientRect(m_WindowHandle, &size);

		// ========== SwapChain ==========
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
			DirectX12System::Get()->GetCommandQueue(),
			m_WindowHandle,
			&scd,
			&scfd,
			nullptr,
			&sc1
		);
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to create DXGI Swap Chain");
		hr = sc1->QueryInterface(&m_SwapChain);
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to retrieve DXGI Swap Chain");
		sc1->Release();

		// ========== Render Target View Descriptor Heap ==========
		D3D12_DESCRIPTOR_HEAP_DESC descRTVheap = {};
		descRTVheap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		descRTVheap.NumDescriptors = GetSwapChainBufferCount();
		descRTVheap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descRTVheap.NodeMask = 0;

		hr = device->CreateDescriptorHeap(&descRTVheap, IID_PPV_ARGS(&m_rtvDescriptorHeap));
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to create Render Target View Descriptor Heap");

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

	inline bool DirectX12Context::RetrieveBuffers()
	{
		for (size_t i = 0; i < GetSwapChainBufferCount(); i++)
		{
			HRESULT hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_Buffers[i]));
			VALIDATE_DXCALL_SUCCESS(hr, "Failed to retrieve D3D12 Buffer");

			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			rtvDesc.Texture2D.PlaneSlice = 0;

			DirectX12System::Get()->GetDevice()->CreateRenderTargetView(
				m_Buffers[i],
				&rtvDesc,
				m_rtvCPUhandles[i]
			);
		}

		return true;
	}

	inline void DirectX12Context::ReleaseBuffers()
	{
		for (size_t i = 0; i < GetSwapChainBufferCount(); i++)
		{
			CHECK_AND_RELEASE_COMPTR(m_Buffers[i]);
		}
	}

	void DirectX12Context::Resize(unsigned int width, unsigned int height)
	{
		m_ShouldResize = true;
		m_Nwidth = width;
		m_Nheight = height;
	}	

	void DirectX12Context::SwapBuffers()
	{
		EndFrame();

		m_SwapChain->Present(1, 0);

		// ========== Handle Resizing between frames ===========
		if (m_ShouldResize)
		{
			ResizeImpl();
		}

		BeginFrame();
	}

	inline void DirectX12Context::BeginFrame()
	{
		auto& commandList = DirectX12System::Get()->InitCommandList();
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
		static float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

		commandList->ClearRenderTargetView(m_rtvCPUhandles[m_CurrentBackBuffer], clearColor, 0, nullptr);

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
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_Buffers[m_CurrentBackBuffer];
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

		DirectX12System::Get()->GetCommandList()->ResourceBarrier(1, &barrier);

		DirectX12System::Get()->ExecuteCommandListAndWait();
	}

	void DirectX12Context::ShutDown()
	{
		ReleaseBuffers();
		if (m_Shader) { delete m_Shader; m_Shader = nullptr; }
		CHECK_AND_RELEASE_COMPTR(m_rtvDescriptorHeap);
		CHECK_AND_RELEASE_COMPTR(m_SwapChain);
		LD_CORE_INFO("Closed DirectX12(D3D12) Graphics Context");
	}

	void DirectX12Context::ResizeImpl()
	{
		m_ShouldResize = false;
		DirectX12System::Get()->Flush(GetSwapChainBufferCount());

		LD_CORE_TRACE("Resizing D3D12 Graphics Context: {0}, {1}", m_Nwidth, m_Nheight);
		ReleaseBuffers();
		HRESULT hr = m_SwapChain->ResizeBuffers(GetSwapChainBufferCount(), m_Nwidth, m_Nheight, DXGI_FORMAT_UNKNOWN,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

		// TODO: Implement proper crash system
		if (FAILED(hr))
		{
			std::exit(-1);
		}
		if (!RetrieveBuffers())
		{
			std::exit(-1);
		}
	}
}