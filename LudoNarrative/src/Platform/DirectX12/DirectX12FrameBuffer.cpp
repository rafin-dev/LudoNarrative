#include "ldpch.h"
#include "DirectX12FrameBuffer.h"

#include "Platform/DirectX12/Utils/DX12Utils.h"
#include "Platform/DirectX12/DirectX12API.h"
#include "Platform/DirectX12/DirectX12Context.h"

namespace Ludo {

	namespace Utils {

		static bool IsDepthFormat(FrameBufferTextureFormat format)
		{
			switch (format)
			{
			case FrameBufferTextureFormat::DEPTH24STENCIL8: return true;
			}

			return false;
		}

		static DXGI_FORMAT GetDXGIFormatFromFrameBufferTextureFormat(FrameBufferTextureFormat fbTxFormat)
		{
			switch (fbTxFormat)
			{
			case Ludo::FrameBufferTextureFormat::RGBA8:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			case Ludo::FrameBufferTextureFormat::DEPTH24STENCIL8:
				return DXGI_FORMAT_D24_UNORM_S8_UINT;
			}

			LD_CORE_ASSERT(false, "Unknown format Speciffied");
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	DirectX12FrameBuffer::DirectX12FrameBuffer(const FrameBufferSpecification& spec)
		: m_Specification(spec)
	{
		LD_PROFILE_FUNCTION();

		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec.TextureFormat))
			{
				m_ColorAttachmentsSpecs.push_back(spec);
			}
			else
			{
				LD_CORE_ASSERT(m_DepthAttachmentSpec.TextureFormat == FrameBufferTextureFormat::None, "Atempt to create Frame Buffer with multiple Depth Attachments");
				m_DepthAttachmentSpec = spec.TextureFormat;
			}
		}

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

		// Attchments
		for (auto& color : m_ColorAttachments)
		{
			CHECK_AND_RELEASE_COMPTR(color);
		}
		m_ColorAttachments.clear();
		CHECK_AND_RELEASE_COMPTR(m_DepthAttachment);

		HRESULT hr = S_OK;
		auto& device = DirectX12API::Get()->GetDevice();

		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 0;
		heapProperties.VisibleNodeMask = 0;

		if (m_ColorAttachmentsSpecs.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentsSpecs.size());

			D3D12_RESOURCE_DESC textureDesc = {};
			textureDesc.MipLevels = 1;
			textureDesc.Format = DXGI_FORMAT_UNKNOWN;
			textureDesc.Width = m_Specification.Width;
			textureDesc.Height = m_Specification.Height;
			textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			textureDesc.DepthOrArraySize = 1;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

			D3D12_CLEAR_VALUE clearValue = {};
			clearValue.Format = DXGI_FORMAT_UNKNOWN;
			clearValue.Color[0] = 0.0f;
			clearValue.Color[1] = 0.0f;
			clearValue.Color[2] = 0.0f;
			clearValue.Color[3] = 0.0f;

			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = DXGI_FORMAT_UNKNOWN;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			rtvDesc.Texture2D.PlaneSlice = 0;

			for (uint32_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				auto DXGIFormat = Utils::GetDXGIFormatFromFrameBufferTextureFormat(m_ColorAttachmentsSpecs[i].TextureFormat);
				textureDesc.Format = DXGIFormat;
				clearValue.Format = DXGIFormat;
				rtvDesc.Format = DXGIFormat;

				hr = device->CreateCommittedResource(&heapProperties,
					D3D12_HEAP_FLAG_NONE,
					&textureDesc,
					D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
					&clearValue, IID_PPV_ARGS(&m_ColorAttachments[i]));
				CHECK_DX12_HRESULT(hr, "Failed to create Render Target Resource with size: [{0}, {1}]", m_Specification.Width, m_Specification.Height);

				device->CreateRenderTargetView(m_ColorAttachments[i], &rtvDesc, m_RenderTargetViews[i]);
			}
		}

		if (!m_ColorAttachments.empty())
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.PlaneSlice = 0;
			srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

			device->CreateShaderResourceView(m_ColorAttachments[0], &srvDesc, m_ImGuiColorCpuHandle);
			m_CurrentBoundColorAttachment = 0;
		}

		if (m_DepthAttachmentSpec.TextureFormat != FrameBufferTextureFormat::None)
		{
			DXGI_FORMAT DXGIFormat = Utils::GetDXGIFormatFromFrameBufferTextureFormat(m_DepthAttachmentSpec.TextureFormat);

			D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
			depthStencilViewDesc.Format = DXGIFormat;
			depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

			D3D12_CLEAR_VALUE depthStencilClearValue = {};
			depthStencilClearValue.Format = DXGIFormat;
			depthStencilClearValue.DepthStencil.Depth = 1.0f;
			depthStencilClearValue.DepthStencil.Stencil = 0;

			D3D12_RESOURCE_DESC depthBufferDesc = {};
			depthBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			depthBufferDesc.Width = m_Specification.Width;
			depthBufferDesc.Height = m_Specification.Height;
			depthBufferDesc.DepthOrArraySize = 1;
			depthBufferDesc.Format = DXGIFormat;
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
				IID_PPV_ARGS(&m_DepthAttachment)
			);
			CHECK_DX12_HRESULT(hr, "Failed to create Depth Stencil Buffer of size: [{0}, {1}]", m_Specification.Width, m_Specification.Height);

			device->CreateDepthStencilView(m_DepthAttachment, &depthStencilViewDesc, m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		}
	}

	void DirectX12FrameBuffer::Bind()
	{
		LD_PROFILE_RENDERER_FUNCTION();

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		for (auto& colorAttachment : m_ColorAttachments)
		{
			barrier.Transition.pResource = colorAttachment;
			DirectX12API::Get()->GetCommandList()->ResourceBarrier(1, &barrier);
		}

		auto& commandList = DirectX12API::Get()->GetCommandList();

		float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		for (auto& rtv : m_RenderTargetViews)
		{
			commandList->ClearRenderTargetView(rtv, color, 0, nullptr);
		}
		D3D12_CPU_DESCRIPTOR_HANDLE* depthStencilView = nullptr;
		if (m_DepthAttachment != nullptr)
		{
			commandList->ClearDepthStencilView(m_DepthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
			depthStencilView = &m_DepthStencilView;
		}

		D3D12_VIEWPORT viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (FLOAT)m_Specification.Width;
		viewport.Height = (FLOAT)m_Specification.Height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		commandList->RSSetViewports(1, &viewport);

		commandList->OMSetRenderTargets(m_ColorAttachments.size(), !m_RenderTargetViews.empty() ? m_RenderTargetViews.data() : nullptr, false, depthStencilView);
	}

	void DirectX12FrameBuffer::Unbind()
	{
		LD_PROFILE_RENDERER_FUNCTION();

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

		DirectX12Context::SetSwapChainRenderTarget();

		for (auto& colorAttachment : m_ColorAttachments)
		{
			barrier.Transition.pResource = colorAttachment;
			DirectX12API::Get()->GetCommandList()->ResourceBarrier(1, &barrier);
		}
	}

	void DirectX12FrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}

	ImTextureID DirectX12FrameBuffer::GetImTextureID(uint32_t index) const
	{
		LD_CORE_ASSERT(index < m_ColorAttachments.size(), "Atempt to bound non existent Color Attachment");

		if (index != m_CurrentBoundColorAttachment)
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.PlaneSlice = 0;
			srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
			DirectX12API::Get()->GetDevice()->CreateShaderResourceView(m_ColorAttachments[index], &srvDesc, m_ImGuiColorCpuHandle);
		}

		return m_ImTextureID;
	}

	bool DirectX12FrameBuffer::Init()
	{
		LD_PROFILE_FUNCTION();

		HRESULT hr = S_OK;
		auto& device = DirectX12API::Get()->GetDevice();

		// ========== RTV decriptor heap ==========
		D3D12_DESCRIPTOR_HEAP_DESC descRTVheap = {};
		descRTVheap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		descRTVheap.NumDescriptors = m_ColorAttachmentsSpecs.size();
		descRTVheap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descRTVheap.NodeMask = 0;

		hr = device->CreateDescriptorHeap(&descRTVheap, IID_PPV_ARGS(&m_rtvDescriptorHeap));
		VALIDATE_DX12_HRESULT(hr, "Failed to create Render Target View Descriptor Heap");

		// ========== CPU Handles ==========
		m_RenderTargetViews.resize(m_ColorAttachmentsSpecs.size());
		D3D12_CPU_DESCRIPTOR_HANDLE firstHandle = m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		UINT RTVstride = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		for (uint32_t i = 0; i < m_ColorAttachmentsSpecs.size(); i++)
		{
			m_RenderTargetViews[i] = firstHandle;
			m_RenderTargetViews[i].ptr += RTVstride * i;
		}

		// ========== Depth Stencil View Descriptor Heap ==========
		D3D12_DESCRIPTOR_HEAP_DESC depthStencilDescHeapDesc = {};
		depthStencilDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		depthStencilDescHeapDesc.NumDescriptors = 1;
		depthStencilDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		depthStencilDescHeapDesc.NodeMask = 0;

		hr = device->CreateDescriptorHeap(&depthStencilDescHeapDesc, IID_PPV_ARGS(&m_DepthStencilDescriptorHeap));
		VALIDATE_DX12_HRESULT(hr, "Failed to create Descriptor Heap for the Depth Stencil View");
		m_DepthStencilView = m_DepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		DirectX12API::Get()->GetSRVDescriptorHeap().Alloc(&m_ImGuiColorCpuHandle, &m_ImGuiColorGpuHandle);
		m_ImTextureID = (ImTextureID)m_ImGuiColorGpuHandle.ptr;

		return true;
	}

	void DirectX12FrameBuffer::ShutDown()
	{
		LD_PROFILE_FUNCTION();

		DirectX12API::Get()->GetSRVDescriptorHeap().Free(m_ImGuiColorCpuHandle, m_ImGuiColorGpuHandle);

		for (auto& colorAttchment : m_ColorAttachments)
		{
			CHECK_AND_RELEASE_COMPTR(colorAttchment);
		}
		CHECK_AND_RELEASE_COMPTR(m_DepthAttachment);

		CHECK_AND_RELEASE_COMPTR(m_rtvDescriptorHeap);
		CHECK_AND_RELEASE_COMPTR(m_DepthStencilDescriptorHeap);
	}

}