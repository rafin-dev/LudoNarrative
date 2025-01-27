#pragma once

#include "Ludo/Renderer/FrameBuffer.h"

#include <imgui.h>

namespace Ludo {

	class DirectX12FrameBuffer : public FrameBuffer
	{
	public:
		DirectX12FrameBuffer(const FrameBufferSpecification& spec);
		~DirectX12FrameBuffer() override;

		void Invalidate();

		void Bind() override;
		void Unbind() override;

		void Resize(uint32_t width, uint32_t height) override;
		int ReadPixel(uint32_t attachmentIndex, uint32_t x, uint32_t y) override;

		ImTextureID GetImTextureID(uint32_t index = 0) override;

		const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

		static std::vector<DXGI_FORMAT> s_CurrentBoundFormats;

	private:
		bool Init();
		void ShutDown();

		bool CreateReadBack(uint32_t index);

		// Descriptor Heaps
		ID3D12DescriptorHeap* m_rtvDescriptorHeap = nullptr;
		ID3D12DescriptorHeap* m_DepthStencilDescriptorHeap = nullptr;

		// Attachments
		std::vector<ID3D12Resource2*> m_ColorAttachments;
		ID3D12Resource2* m_DepthAttachment = nullptr;

		std::vector<ID3D12Resource2*> m_ReadBacks;
		std::vector<size_t> m_ReadBackAlignment;

		std::vector<DXGI_FORMAT> m_Formats = std::vector<DXGI_FORMAT>(9, DXGI_FORMAT_UNKNOWN);
		std::vector<size_t> m_Sizes;

		// Views
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_RenderTargetViews;
		D3D12_CPU_DESCRIPTOR_HANDLE m_DepthStencilView;

		// Shader Resource View for ImGui
		D3D12_CPU_DESCRIPTOR_HANDLE m_ImGuiColorCpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE m_ImGuiColorGpuHandle;
		ImTextureID m_ImTextureID;
		uint32_t m_CurrentBoundColorAttachment = 0;

		// Specifications
		FrameBufferSpecification m_Specification;
		std::vector<FrameBufferTextureSpecification> m_ColorAttachmentsSpecs;
		FrameBufferTextureSpecification m_DepthAttachmentSpec = FrameBufferTextureFormat::None;
	};

}