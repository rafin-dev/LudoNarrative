#pragma once

#include "Ludo/Renderer/FrameBuffer.h"

#include <imgui/imgui.h>

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

		ImTextureID GetImTextureID(uint32_t index = 0) const override;

		const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

	private:
		bool Init();
		void ShutDown();

		// Descriptor Heaps
		ID3D12DescriptorHeap* m_rtvDescriptorHeap = nullptr;
		ID3D12DescriptorHeap* m_DepthStencilDescriptorHeap = nullptr;

		// Attachments
		std::vector<ID3D12Resource2*> m_ColorAttachments;
		ID3D12Resource2* m_DepthAttachment = nullptr;

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