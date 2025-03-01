#include "ldpch.h"
#include "DirectX11FrameBuffer.h"

#include "DirectX11API.h"
#include "Utils/DirectX11Utils.h"

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
			case FrameBufferTextureFormat::RGBA8:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			case FrameBufferTextureFormat::RED_INTEGER:
				return DXGI_FORMAT_R32_SINT;
			case FrameBufferTextureFormat::DEPTH24STENCIL8:
				return DXGI_FORMAT_D24_UNORM_S8_UINT;
			}

			LD_CORE_ASSERT(false, "Unknown format Speciffied");
			return DXGI_FORMAT_UNKNOWN;
		}

	}

	ID3D11RenderTargetView* DirectX11FrameBuffer::SwapChainTarget = nullptr;
	ID3D11DepthStencilView* DirectX11FrameBuffer::SwapChainDepthStencil = nullptr;
	ID3D11BlendState* DirectX11FrameBuffer::SwapChainBlendSate = nullptr;
	D3D11_VIEWPORT DirectX11FrameBuffer::SwapChainViewport;

	DirectX11FrameBuffer::DirectX11FrameBuffer(const FrameBufferSpecification& spec)
		: m_Specification(spec)
	{
		Init();
	}
	
	DirectX11FrameBuffer::~DirectX11FrameBuffer()
	{
		ShutDown();
	}

	void DirectX11FrameBuffer::Bind()
	{
		LD_PROFILE_RENDERER_FUNCTION();

		auto& deviceContext = DirectX11API::Get()->GetDeviceContext();

		for (uint32_t i = 0; i < m_ColorAttachmentRTVs.size(); i++)
		{
			deviceContext->ClearRenderTargetView(m_ColorAttachmentRTVs[i], (float*)&m_ColorAttachmentsSpecs[i].ClearColor);
		}
		deviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = m_Specification.Width;
		viewport.Height = m_Specification.Height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		deviceContext->RSSetViewports(1, &viewport);

		deviceContext->OMSetBlendState(m_BlendState, nullptr, 0xffffffff);
		
		deviceContext->OMSetRenderTargets(m_ColorAttachmentRTVs.size(), m_ColorAttachmentRTVs.data(), m_DepthStencilView);
	}

	void DirectX11FrameBuffer::Unbind()
	{
		LD_PROFILE_RENDERER_FUNCTION();

		auto& deviceContext = DirectX11API::Get()->GetDeviceContext();

		deviceContext->OMSetRenderTargets(1, &SwapChainTarget, SwapChainDepthStencil);
		deviceContext->OMSetBlendState(SwapChainBlendSate, nullptr, 0xffffffff);
		deviceContext->RSSetViewports(1, &SwapChainViewport);

		for (uint32_t i = 0; i < m_ColorAttachmentsSpecs.size(); i++)
		{
			if (m_ColorAttachmentsSpecs[i].AllowReadBack)
			{
				D3D11_BOX box = {};
				box.left = 0;
				box.top = 0;
				box.front = 0;

				box.right = m_Specification.Width;
				box.bottom = m_Specification.Height;
				box.back = 1;

				deviceContext->CopySubresourceRegion(m_ReadBacks[i], 0, 0, 0, 0, m_ColorAttachments[i].Texture, 0, &box);
			}
		}
	}

	void DirectX11FrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}

	int DirectX11FrameBuffer::ReadPixel(uint32_t attachmentIndex, uint32_t x, uint32_t y)
	{
		LD_PROFILE_RENDERER_FUNCTION();

		LD_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Atempt to read non existant color attachment");
		LD_CORE_ASSERT(x < m_Specification.Width && y < m_Specification.Height, "Atempt to read outside of the texture");
		LD_CORE_ASSERT(m_ColorAttachmentsSpecs[attachmentIndex].AllowReadBack, "Atempt to read from non ReadBack Color Attachment");

		auto& deviceContext = DirectX11API::Get()->GetDeviceContext();

		D3D11_MAPPED_SUBRESOURCE mapped = {};

		HRESULT hr = deviceContext->Map(m_ReadBacks[attachmentIndex], 0, D3D11_MAP_READ, 0, &mapped);
		LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to map Color Attachment");

		int32_t* data = (int32_t*)mapped.pData;
		int32_t pixel = data[(mapped.RowPitch / 4) * y + x];

		deviceContext->Unmap(m_ReadBacks[attachmentIndex], 0);

		return pixel;
	}

	void DirectX11FrameBuffer::ClearDepthAttachment(float clearValue)
	{
		LD_PROFILE_RENDERER_FUNCTION();

		LD_CORE_ASSERT(m_DepthAttachment != nullptr, "Atempt to use Depth Attachment on a frame buffer with no Depth Attachment");

		DirectX11API::Get()->GetDeviceContext()->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	ImTextureID DirectX11FrameBuffer::GetImTextureID(uint32_t index)
	{
		LD_CORE_ASSERT(index < m_ColorAttachments.size(), "Atempt to use non existant Color Attachment");
		return (ImTextureID)m_ColorAttachments[index].ShaderResourceView;
	}

	void DirectX11FrameBuffer::Init()
	{
		LD_PROFILE_FUNCTION();

		D3D11_RENDER_TARGET_BLEND_DESC blendOn = {};
		blendOn.BlendEnable = true;
		blendOn.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendOn.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendOn.BlendOp = D3D11_BLEND_OP_ADD;
		blendOn.SrcBlendAlpha = D3D11_BLEND_ONE;
		blendOn.DestBlendAlpha = D3D11_BLEND_ZERO;
		blendOn.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendOn.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		D3D11_RENDER_TARGET_BLEND_DESC blendoff = {};
		blendoff.BlendEnable = false;
		blendoff.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		D3D11_BLEND_DESC BlendDesc = {};
		BlendDesc.AlphaToCoverageEnable = false;
		BlendDesc.IndependentBlendEnable = true;
		for (auto& rtb : BlendDesc.RenderTarget)
		{
			rtb = blendoff;
		}

		for (uint32_t i = 0; i < m_Specification.Attachments.Attachments.size(); i++)
		{
			if (!Utils::IsDepthFormat(m_Specification.Attachments.Attachments[i].TextureFormat))
			{
				m_ColorAttachmentsSpecs.push_back(m_Specification.Attachments.Attachments[i]);
			}
			else
			{
				LD_CORE_ASSERT(m_DepthAttachmentSpec.TextureFormat == FrameBufferTextureFormat::None, "Atempt to create Frame Buffer with multiple Depth Attachments");
				m_DepthAttachmentSpec = m_Specification.Attachments.Attachments[i].TextureFormat;
			}

			if (m_Specification.Attachments.Attachments[i].TextureFormat != FrameBufferTextureFormat::RED_INTEGER)
			{
				BlendDesc.RenderTarget[i] = blendOn;
			}
			else
			{
				BlendDesc.RenderTarget[i] = blendoff;
			}
		}

		HRESULT hr = DirectX11API::Get()->GetDevice()->CreateBlendState(&BlendDesc, &m_BlendState);
		LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to create Blend State");

		Invalidate();
	}

	void DirectX11FrameBuffer::ShutDown()
	{
		LD_PROFILE_FUNCTION();

		for (auto& color : m_ColorAttachments)
		{
			CHECK_AND_RELEASE_COMPTR(color.ShaderResourceView);
			CHECK_AND_RELEASE_COMPTR(color.Texture);
		}
		for (auto& rtv : m_ColorAttachmentRTVs)
		{
			CHECK_AND_RELEASE_COMPTR(rtv);
		}
		for (auto& rb : m_ReadBacks)
		{
			CHECK_AND_RELEASE_COMPTR(rb);
		}

		CHECK_AND_RELEASE_COMPTR(m_DepthStencilView);
		CHECK_AND_RELEASE_COMPTR(m_DepthAttachment);
		CHECK_AND_RELEASE_COMPTR(m_BlendState);
	}

	void DirectX11FrameBuffer::Invalidate()
	{
		LD_PROFILE_FUNCTION();

		for (auto& color : m_ColorAttachments)
		{
			CHECK_AND_RELEASE_COMPTR(color.ShaderResourceView);
			CHECK_AND_RELEASE_COMPTR(color.Texture);
		}
		for (auto& rtv : m_ColorAttachmentRTVs)
		{
			CHECK_AND_RELEASE_COMPTR(rtv);
		}
		for (auto& rb : m_ReadBacks)
		{
			CHECK_AND_RELEASE_COMPTR(rb);
		}
		
		m_ColorAttachments.clear();
		m_ColorAttachmentRTVs.clear();
		m_ReadBacks.clear();
		CHECK_AND_RELEASE_COMPTR(m_DepthAttachment);
		CHECK_AND_RELEASE_COMPTR(m_DepthStencilView);

		HRESULT hr = S_OK;
		auto& device = DirectX11API::Get()->GetDevice();
		auto& deviceContext = DirectX11API::Get()->GetDeviceContext();

		if (m_ColorAttachmentsSpecs.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentsSpecs.size());
			m_ColorAttachmentRTVs.resize(m_ColorAttachmentsSpecs.size());
			m_ReadBacks.resize(m_ColorAttachmentsSpecs.size());

			D3D11_TEXTURE2D_DESC textureDesc = {};
			textureDesc.Width = m_Specification.Width;
			textureDesc.Height = m_Specification.Height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_UNKNOWN;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			rtvDesc.Format = DXGI_FORMAT_UNKNOWN;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = -1;
			srvDesc.Texture2D.MostDetailedMip = 0;

			for (uint32_t i = 0; i < m_ColorAttachmentsSpecs.size(); i++)
			{
				auto format = Utils::GetDXGIFormatFromFrameBufferTextureFormat(m_ColorAttachmentsSpecs[i].TextureFormat);
				textureDesc.Format = format;
				rtvDesc.Format = format;
				srvDesc.Format = format;

				hr = device->CreateTexture2D(&textureDesc, nullptr, &m_ColorAttachments[i].Texture);
				LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to create Color Attachment Texture2D");
				
				hr = device->CreateRenderTargetView(m_ColorAttachments[i].Texture, &rtvDesc, &m_ColorAttachmentRTVs[i]);
				LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to create Color Attachment Render Target View");

				hr = device->CreateShaderResourceView(m_ColorAttachments[i].Texture, &srvDesc, &m_ColorAttachments[i].ShaderResourceView);
				LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to create Color Attachment Shader Resource View");

				if (m_ColorAttachmentsSpecs[i].AllowReadBack)
				{
					textureDesc.Usage = D3D11_USAGE_STAGING;
					textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
					textureDesc.BindFlags = 0;

					hr = device->CreateTexture2D(&textureDesc, nullptr, &m_ReadBacks[i]);
					LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to create ReadBack Buffer for Color Attachment");

					textureDesc.Usage = D3D11_USAGE_DEFAULT;
					textureDesc.CPUAccessFlags = 0;
					textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
				}
			}
		}

		if (m_DepthAttachmentSpec.TextureFormat != FrameBufferTextureFormat::None)
		{
			auto format = Utils::GetDXGIFormatFromFrameBufferTextureFormat(m_DepthAttachmentSpec.TextureFormat);

			D3D11_TEXTURE2D_DESC textureDesc = {};
			textureDesc.Width = m_Specification.Width;
			textureDesc.Height = m_Specification.Height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = format;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

			hr = device->CreateTexture2D(&textureDesc, nullptr, &m_DepthAttachment);
			LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to create Depth Stencil Buffer");

			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.Format = format;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;

			hr = device->CreateDepthStencilView((ID3D11Resource*)m_DepthAttachment, &dsvDesc, &m_DepthStencilView);
			LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to create Depth Stencil View");
		}
	}

}