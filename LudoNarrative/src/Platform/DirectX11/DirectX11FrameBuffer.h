#pragma once

#include "Ludo/Renderer/FrameBuffer.h"

#include <vector>

#include <d3d11.h>

namespace Ludo {

	class DirectX11FrameBuffer : public FrameBuffer
	{
	public:
		DirectX11FrameBuffer(const FrameBufferSpecification& spec);
		~DirectX11FrameBuffer() override;

		const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

		void Bind() override;
		void Unbind() override;

		void Resize(uint32_t width, uint32_t height) override;
		int ReadPixel(uint32_t attachmentIndex, uint32_t x, uint32_t y) override;
		void ClearDepthAttachment(float clearValue) override;

		ImTextureID GetImTextureID(uint32_t index = 0) override;

		static ID3D11RenderTargetView* SwapChainTarget;
		static ID3D11DepthStencilView* SwapChainDepthStencil;
		static ID3D11BlendState* SwapChainBlendSate;
		static D3D11_VIEWPORT SwapChainViewport;

	private:
		void Init();
		void ShutDown();
		void Invalidate();

		struct ColorAttachment
		{
			ID3D11Texture2D* Texture = nullptr;
			ID3D11ShaderResourceView* ShaderResourceView = nullptr;
		};
		
		std::vector<ColorAttachment> m_ColorAttachments;
		std::vector<ID3D11RenderTargetView*> m_ColorAttachmentRTVs;
		ID3D11Texture2D* m_DepthAttachment = nullptr;
		ID3D11DepthStencilView* m_DepthStencilView = nullptr;

		ID3D11BlendState* m_BlendState = nullptr;

		std::vector<ID3D11Texture2D*> m_ReadBacks;

		FrameBufferSpecification m_Specification;

		std::vector<FrameBufferTextureSpecification> m_ColorAttachmentsSpecs;
		FrameBufferTextureSpecification m_DepthAttachmentSpec = FrameBufferTextureFormat::None;
	};

}