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

		ImTextureID GetImTextureID() const override { return m_ImTextureID; }

		const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

	private:
		bool Init();
		void ShutDown();

		// Descriptor Heaps
		ID3D12DescriptorHeap* m_rtvDescriptorHeap = nullptr;
		ID3D12DescriptorHeap* m_DepthStencilDescriptorHeap = nullptr;

		// Resources
		ID3D12Resource2* m_ColorBuffer = nullptr;
		ID3D12Resource2* m_DepthBuffer = nullptr;

		// Views
		D3D12_CPU_DESCRIPTOR_HANDLE m_RenderTargetView;
		D3D12_CPU_DESCRIPTOR_HANDLE m_DepthStencilView;

		// Shader Resource View for ImGui
		D3D12_CPU_DESCRIPTOR_HANDLE m_ColorCpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE m_ColorGpuHandle;
		ImTextureID m_ImTextureID;

		// Specification
		FrameBufferSpecification m_Specification;
	};

}