#include "ldpch.h"
#include "DirectX12Context.h"

#include "DirectX12Renderer.h"
#include "DX12Utils.h"

#include "Platform/Windows/WindowsWindow.h"
#include "Ludo/Application.h"

namespace Ludo {

	DirectX12Context::DirectX12Context(HWND window)
		: m_WindowHandle(window)
	{

	}

	DirectX12Context::~DirectX12Context()
	{
		EndFrame();
		DirectX12Renderer::Get()->Flush(GetSwapChainBufferCount());

		ShutDown();
	}

	static D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	static size_t countVerticies = 0;

	class Shader
	{
	public:
		Shader() {}
		Shader(std::string_view name)
		{
			wchar_t moduleFileName[MAX_PATH];
			GetModuleFileNameW(nullptr, moduleFileName, MAX_PATH);

			std::filesystem::path shaderPath = moduleFileName;
			shaderPath.remove_filename();
			shaderPath += "../LudoNarrative";
			shaderPath = shaderPath / name;
			std::ifstream shaderIn(shaderPath, std::ios::binary);

			if (shaderIn.is_open())
			{
				m_Size = std::filesystem::file_size(shaderPath);
				m_Data = malloc(m_Size);
				if (m_Data != nullptr)
				{
					shaderIn.read((char*)m_Data, m_Size);
					LD_CORE_TRACE("{0}", m_Size);
				}
			}
		}
		~Shader()
		{
			if (m_Data != nullptr) { free(m_Data); m_Data = nullptr; }
		}

		inline const void* GetBuffer() { return m_Data; }
		inline const SIZE_T GetSize() { return m_Size; }

	private:
		void* m_Data = nullptr;
		SIZE_T m_Size;
	};

	bool DirectX12Context::Init()
	{
		HRESULT hr = S_OK;
		auto& factory = DirectX12Renderer::Get()->GetDXGIFactory();
		auto& device = DirectX12Renderer::Get()->GetDevice();
		m_Window = (WindowsWindow*)GetWindowLongPtr(m_WindowHandle, GWLP_USERDATA);
		RECT size = {};
		GetClientRect(m_WindowHandle, &size);

		// SwapChain
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

		// Render Target View Descriptor Heap
		D3D12_DESCRIPTOR_HEAP_DESC descRTVheap = {};
		descRTVheap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		descRTVheap.NumDescriptors = GetSwapChainBufferCount();
		descRTVheap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descRTVheap.NodeMask = 0;

		hr = device->CreateDescriptorHeap(&descRTVheap, IID_PPV_ARGS(&m_rtvDescriptorHeap));
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to create Render Target View Descriptor Heap");

		// Render Target View CPU Descriptor handle
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

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Vertex data
		struct Vertex
		{
			float x, y;
		};
		Vertex verticies[] =
		{
			{ -1.f, -1.f },
			{  0.f,  1.f },
			{  1.f, -1.f }
		};
		countVerticies = _countof(verticies);

		D3D12_INPUT_ELEMENT_DESC vertexLayout[] =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_HEAP_PROPERTIES heapPropertiesUpload = {};
		heapPropertiesUpload.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapPropertiesUpload.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapPropertiesUpload.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapPropertiesUpload.CreationNodeMask = 0;
		heapPropertiesUpload.VisibleNodeMask = 0;

		D3D12_HEAP_PROPERTIES heapPropertiesDefault = {};
		heapPropertiesDefault.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapPropertiesDefault.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapPropertiesDefault.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapPropertiesDefault.CreationNodeMask = 0;
		heapPropertiesDefault.VisibleNodeMask = 0;

		// Upload & vertex buffer
		D3D12_RESOURCE_DESC resourceDescription = {};
		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDescription.Width = 1024;
		resourceDescription.Height = 1;
		resourceDescription.DepthOrArraySize = 1;
		resourceDescription.MipLevels = 1;
		resourceDescription.Format = DXGI_FORMAT_UNKNOWN;
		resourceDescription.SampleDesc.Count = 1;
		resourceDescription.SampleDesc.Quality = 0;
		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

		// Create buffers
		hr = device->CreateCommittedResource(&heapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &resourceDescription, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&m_UploadBuffer));
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to create Upload Buffer");

		hr = device->CreateCommittedResource(&heapPropertiesDefault, D3D12_HEAP_FLAG_NONE, &resourceDescription, D3D12_RESOURCE_STATE_COMMON,
			nullptr, IID_PPV_ARGS(&m_VertexBuffer));
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to create Vertex Buffer");

		// Vertex Buffer View
		vertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = _countof(verticies) * sizeof(Vertex);
		vertexBufferView.StrideInBytes = sizeof(Vertex);

		// Copy void* --> CPU Resource
		void* uploadBufferAddress;
		D3D12_RANGE uploadRange = {};
		uploadRange.Begin = 0;
		uploadRange.End = 1023;

		hr = m_UploadBuffer->Map(0, &uploadRange, &uploadBufferAddress);
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to map Resource");

		memcpy(uploadBufferAddress, verticies, sizeof(verticies));

		m_UploadBuffer->Unmap(0, &uploadRange);

		// Copy CPU Resource --> GPU Resource
		auto& commandList = DirectX12Renderer::Get()->InitCommandList();

		commandList->CopyBufferRegion(m_VertexBuffer, 0, m_UploadBuffer, 0, 1024);

		DirectX12Renderer::Get()->ExecuteCommandList();

		Shader RootSignatureShader("RootSignature.cso");
		Shader VertexShader("VertexShader.cso");
		Shader PixelShader("PixelShader.cso");

		// Create Root Signature
		hr = device->CreateRootSignature(0, RootSignatureShader.GetBuffer(), RootSignatureShader.GetSize(), IID_PPV_ARGS(&m_RootSignature));
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to create Root Signature");

		// Pipeline State
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDescription = {};

		// Root Signature
		pipelineStateDescription.pRootSignature = m_RootSignature;
		// Input Layout
		pipelineStateDescription.InputLayout.NumElements = 1;
		pipelineStateDescription.InputLayout.pInputElementDescs = vertexLayout;
		pipelineStateDescription.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		// Vertex shader
		pipelineStateDescription.VS.BytecodeLength = VertexShader.GetSize();
		pipelineStateDescription.VS.pShaderBytecode = VertexShader.GetBuffer();
		// Pixel shader
		pipelineStateDescription.PS.BytecodeLength = PixelShader.GetSize();
		pipelineStateDescription.PS.pShaderBytecode = PixelShader.GetBuffer();
		// Domain shader
		pipelineStateDescription.DS.BytecodeLength = 0;
		pipelineStateDescription.DS.pShaderBytecode = nullptr;
		// Hull shader
		pipelineStateDescription.HS.BytecodeLength = 0;
		pipelineStateDescription.HS.pShaderBytecode = nullptr;
		// Geometry shader
		pipelineStateDescription.GS.BytecodeLength = 0;
		pipelineStateDescription.GS.pShaderBytecode = nullptr;
		// Rasterizer
		pipelineStateDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateDescription.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		pipelineStateDescription.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		pipelineStateDescription.RasterizerState.FrontCounterClockwise = FALSE;
		pipelineStateDescription.RasterizerState.DepthBias = 0;
		pipelineStateDescription.RasterizerState.DepthBiasClamp = 0.0f;
		pipelineStateDescription.RasterizerState.SlopeScaledDepthBias = 0.0f;
		pipelineStateDescription.RasterizerState.DepthClipEnable = FALSE;
		pipelineStateDescription.RasterizerState.MultisampleEnable = FALSE;
		pipelineStateDescription.RasterizerState.AntialiasedLineEnable = FALSE;
		pipelineStateDescription.RasterizerState.ForcedSampleCount = 0;
		// StreamOutput
		pipelineStateDescription.StreamOutput.pSODeclaration = nullptr;
		pipelineStateDescription.StreamOutput.NumEntries = 0;
		pipelineStateDescription.StreamOutput.pBufferStrides = nullptr;
		pipelineStateDescription.StreamOutput.NumStrides = 0;
		pipelineStateDescription.StreamOutput.RasterizedStream = 0;
		// NumRenderTargets
		pipelineStateDescription.NumRenderTargets = 1;
		// RTVFormats
		pipelineStateDescription.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		// DSVFormat
		pipelineStateDescription.DSVFormat = DXGI_FORMAT_UNKNOWN;
		// BlendState
		pipelineStateDescription.BlendState.AlphaToCoverageEnable = FALSE;
		pipelineStateDescription.BlendState.IndependentBlendEnable = FALSE;
		// RenderTarget BlendState
		pipelineStateDescription.BlendState.RenderTarget[0].BlendEnable = TRUE;
		pipelineStateDescription.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		pipelineStateDescription.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		pipelineStateDescription.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		pipelineStateDescription.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
		pipelineStateDescription.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		pipelineStateDescription.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		pipelineStateDescription.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
		pipelineStateDescription.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
		pipelineStateDescription.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		// DepthStencilState
		pipelineStateDescription.DepthStencilState.DepthEnable = FALSE;
		pipelineStateDescription.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		pipelineStateDescription.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		pipelineStateDescription.DepthStencilState.StencilEnable = FALSE;
		pipelineStateDescription.DepthStencilState.StencilReadMask = 0;
		pipelineStateDescription.DepthStencilState.StencilWriteMask = 0;
		pipelineStateDescription.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		pipelineStateDescription.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pipelineStateDescription.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pipelineStateDescription.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		pipelineStateDescription.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		pipelineStateDescription.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pipelineStateDescription.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pipelineStateDescription.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		// SampleMask
		pipelineStateDescription.SampleMask = 0xFFFFFFFF;
		// SampleDesc
		pipelineStateDescription.SampleDesc.Count = 1;
		pipelineStateDescription.SampleDesc.Quality = 0;
		// NodeMask
		pipelineStateDescription.NodeMask = 0;
		// Chached PSO
		pipelineStateDescription.CachedPSO.CachedBlobSizeInBytes = 0;
		pipelineStateDescription.CachedPSO.pCachedBlob = nullptr;
		// Flags
		pipelineStateDescription.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		hr = device->CreateGraphicsPipelineState(&pipelineStateDescription, IID_PPV_ARGS(&m_PipelineStateObject));
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to create Graphics PipelineState Object");

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

			DirectX12Renderer::Get()->GetDevice()->CreateRenderTargetView(
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

		// Resize between frames
		if (m_ShouldResize)
		{
			ResizeImpl();
		}

		BeginFrame();
		
		auto& commandList = DirectX12Renderer::Get()->GetCommandList();

		// PipelineState and RootSignature
		commandList->SetPipelineState(m_PipelineStateObject);
		commandList->SetGraphicsRootSignature(m_RootSignature);

		// Input Assembler
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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

		// Root Arguments
		static float color[] = { 1.0f, 0.0f, 0.0f };
		static int pukeState = 0;

		color[pukeState] += 0.01f;
		if (color[pukeState] > 1.0f)
		{
			pukeState++;
			if (pukeState == 4)
			{
				color[0] = 0.0f;
				color[1] = 0.0f;
				color[2] = 0.0f;
				pukeState = 0;
			}
		}

		commandList->SetGraphicsRoot32BitConstants(0, 3, color, 0);

		// Draw Call
		commandList->DrawInstanced(countVerticies, 1, 0, 0);
	}

	inline void DirectX12Context::BeginFrame()
	{
		auto& CommandList = DirectX12Renderer::Get()->InitCommandList();
		m_CurrentBackBuffer = m_SwapChain->GetCurrentBackBufferIndex();

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_Buffers[m_CurrentBackBuffer];
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		CommandList->ResourceBarrier(1, &barrier);

		static float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0 };
		static int count = 0;
		//count++;

		if (count > 60)
		{
			count = 0;
			for (int i = 0; i < 4; i++)
			{
				clearColor[i] = std::rand() / double(RAND_MAX);
			}
		}
		CommandList->ClearRenderTargetView(m_rtvCPUhandles[m_CurrentBackBuffer], clearColor, 0, nullptr);

		CommandList->OMSetRenderTargets(1, &m_rtvCPUhandles[m_CurrentBackBuffer], false, nullptr);
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

		DirectX12Renderer::Get()->GetCommandList()->ResourceBarrier(1, &barrier);

		DirectX12Renderer::Get()->ExecuteCommandList();
	}

	void DirectX12Context::ShutDown()
	{
		ReleaseBuffers();
		CHECK_AND_RELEASE_COMPTR(m_PipelineStateObject);
		CHECK_AND_RELEASE_COMPTR(m_RootSignature);
		CHECK_AND_RELEASE_COMPTR(m_UploadBuffer);
		CHECK_AND_RELEASE_COMPTR(m_VertexBuffer);
		CHECK_AND_RELEASE_COMPTR(m_rtvDescriptorHeap);
		CHECK_AND_RELEASE_COMPTR(m_SwapChain);
		LD_CORE_INFO("Closed DirectX12(D3D12) Graphics Context");
	}

	void DirectX12Context::ResizeImpl()
	{
		m_ShouldResize = false;
		DirectX12Renderer::Get()->Flush(GetSwapChainBufferCount());

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