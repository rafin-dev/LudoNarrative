#include "ldpch.h"
#include "DirectX12FrameBuffer.h"

#include "Platform/DirectX12/Utils/DX12Utils.h"
#include "Platform/DirectX12/DirectX12API.h"
#include "Platform/DirectX12/DirectX12Context.h"

namespace Ludo {

	DirectX12FrameBuffer::DirectX12FrameBuffer(const FrameBufferSpecification& spec)
		: m_Specification(spec)
	{
		LD_PROFILE_FUNCTION();

		Init();
		Invalidate();
	}

	DirectX12FrameBuffer::~DirectX12FrameBuffer()
	{
		ShutDown();
	}

	void DirectX12FrameBuffer::Invalidate()
	{
		LD_PROFILE_FUNCTION();

		// Releasing Buffers
		CHECK_AND_RELEASE_COMPTR(m_ColorBuffer);
		CHECK_AND_RELEASE_COMPTR(m_DepthBuffer);

		auto& device = DirectX12API::Get()->GetDevice();

		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 0;
		heapProperties.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width = m_Specification.Width;
		textureDesc.Height = m_Specification.Height;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		clearValue.Color[0] = 0.0f;
		clearValue.Color[1] = 0.0f;
		clearValue.Color[2] = 0.0f;
		clearValue.Color[3] = 0.0f;

		HRESULT hr = device->CreateCommittedResource(&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
			&clearValue, IID_PPV_ARGS(&m_ColorBuffer));
		CHECK_DX12_HRESULT(hr, "Failed to create Render Target Resource with size: [{0}, {1}]", m_Specification.Width, m_Specification.Height);

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;

		DirectX12API::Get()->GetDevice()->CreateRenderTargetView(m_ColorBuffer, &rtvDesc, m_RenderTargetView);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.PlaneSlice = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		DirectX12API::Get()->GetDevice()->CreateShaderResourceView(m_ColorBuffer, &srvDesc, m_ColorCpuHandle);

		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

		D3D12_CLEAR_VALUE depthStencilClearValue = {};
		depthStencilClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilClearValue.DepthStencil.Depth = 1.0f;
		depthStencilClearValue.DepthStencil.Stencil = 0;

		D3D12_RESOURCE_DESC depthBufferDesc = {};
		depthBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthBufferDesc.Width = m_Specification.Width;
		depthBufferDesc.Height = m_Specification.Height;
		depthBufferDesc.DepthOrArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		hr = device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&depthBufferDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthStencilClearValue,
			IID_PPV_ARGS(&m_DepthBuffer)
		);
		CHECK_DX12_HRESULT(hr, "Failed to create Depth Stencil Buffer of size: [{0}, {1}]", m_Specification.Width, m_Specification.Height);

		device->CreateDepthStencilView(m_DepthBuffer, &depthStencilViewDesc, m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	}

	void DirectX12FrameBuffer::Bind()
	{
		LD_PROFILE_RENDERER_FUNCTION();

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = m_ColorBuffer;
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		DirectX12API::Get()->GetCommandList()->ResourceBarrier(1, &barrier);

		auto& commandList = DirectX12API::Get()->GetCommandList();

		float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		commandList->ClearRenderTargetView(m_RenderTargetView, color, 0, nullptr);
		commandList->ClearDepthStencilView(m_DepthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		D3D12_VIEWPORT viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (FLOAT)m_Specification.Width;
		viewport.Height = (FLOAT)m_Specification.Height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		commandList->RSSetViewports(1, &viewport);

		commandList->OMSetRenderTargets(1, &m_RenderTargetView, false, &m_DepthStencilView);
	}

	void DirectX12FrameBuffer::Unbind()
	{
		LD_PROFILE_RENDERER_FUNCTION();

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = m_ColorBuffer;
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

		DirectX12Context::SetSwapChainRenderTarget();

		DirectX12API::Get()->GetCommandList()->ResourceBarrier(1, &barrier);
	}

	void DirectX12FrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}

	bool DirectX12FrameBuffer::Init()
	{
		LD_PROFILE_FUNCTION();

		HRESULT hr = S_OK;
		auto& device = DirectX12API::Get()->GetDevice();

		// ========== RTV decriptor heap ==========
		D3D12_DESCRIPTOR_HEAP_DESC descRTVheap = {};
		descRTVheap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		descRTVheap.NumDescriptors = 2;
		descRTVheap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descRTVheap.NodeMask = 0;

		hr = device->CreateDescriptorHeap(&descRTVheap, IID_PPV_ARGS(&m_rtvDescriptorHeap));
		VALIDATE_DX12_HRESULT(hr, "Failed to create Render Target View Descriptor Heap");
		m_RenderTargetView = m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		// ========== Depth Stencil View Descriptor Heap ==========
		D3D12_DESCRIPTOR_HEAP_DESC depthStencilDescHeapDesc = {};
		depthStencilDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		depthStencilDescHeapDesc.NumDescriptors = 1;
		depthStencilDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		depthStencilDescHeapDesc.NodeMask = 0;

		hr = device->CreateDescriptorHeap(&depthStencilDescHeapDesc, IID_PPV_ARGS(&m_DepthStencilDescriptorHeap));
		VALIDATE_DX12_HRESULT(hr, "Failed to create Descriptor Heap for the Depth Stencil View");
		m_DepthStencilView = m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		DirectX12API::Get()->GetSRVDescriptorHeap().Alloc(&m_ColorCpuHandle, &m_ColorGpuHandle);
		m_ImTextureID = (ImTextureID)m_ColorGpuHandle.ptr;

		return true;
	}

	void DirectX12FrameBuffer::ShutDown()
	{
		LD_PROFILE_FUNCTION();

		CHECK_AND_RELEASE_COMPTR(m_rtvDescriptorHeap);
		CHECK_AND_RELEASE_COMPTR(m_DepthStencilDescriptorHeap);

		CHECK_AND_RELEASE_COMPTR(m_ColorBuffer);
		CHECK_AND_RELEASE_COMPTR(m_DepthBuffer);
	}

}