#pragma once

#include "Ludo/Renderer/RendererAPI.h"
#include "Platform/DirectX12/Utils/DX12CommandHelper.h"
#include "Platform/DirectX12/Utils/DX12SRVDescriptorHeap.h"

#include "d3d12.h"

namespace Ludo {

	class DirectX12API : public RendererAPI
	{
	public:
		virtual bool Init() override;
		virtual void ShutDown() override;

		virtual void SetClearColor(const DirectX::XMFLOAT4& color) override;
		virtual const DirectX::XMFLOAT4& GetClearColor() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;

		virtual void BeginImGui() override;
		virtual void EndImGui() override;

		static DirectX12API* Get();

		inline auto* const& GetDevice() { return m_Device; }
		inline auto* const& GetDXGIFactory() { return m_DXGIFactory; }
		inline auto* const& GetCommandQueue() { return m_GraphicsCommands.GetCommandQueue(); }
		inline auto* const& GetCommandList() { return m_GraphicsCommands.GetCommandList(); }
		inline auto &const GetSRVDescriptorHeap() { return m_SrvDescriptorHeap; }

		inline auto& GetCopyCommandHelper() { return m_CopyCommands; }

		inline ID3D12GraphicsCommandList6* const& InitCommandList() { return m_GraphicsCommands.InitCommandList(); }

		void ImediateCopyBufferRegion(ID3D12Resource2* dest, size_t destOffset,
			ID3D12Resource2* src, size_t srcOffset, size_t size);

		void ImediateCopyTextureRegion(const D3D12_TEXTURE_COPY_LOCATION& dest, uint32_t destX, uint32_t destY, uint32_t destZ,
			const D3D12_TEXTURE_COPY_LOCATION& src, const D3D12_BOX& srcBox);

		inline void ExecuteCommandListAndWait()
		{
			m_GraphicsCommands.ExecuteCommandListAndWait();
		}

		inline void Flush(uint32_t count)
		{
			m_GraphicsCommands.Flush(count);
			m_CopyCommands.Flush(count);
		}

	private:
		bool InitImGui();
		void CloseImGui();

		bool m_ImGuiInitialized = false;

		ID3D12Device8* m_Device = nullptr;

		DX12CommandHelper m_GraphicsCommands;
		DX12CommandHelper m_CopyCommands;

		IDXGIFactory7* m_DXGIFactory = nullptr;

		DX12SRVDescriptorHeap m_SrvDescriptorHeap;

		DirectX::XMFLOAT4 m_ClearColor = { 0.0f, 0.0f, 0.0f, 0.0f };

#ifdef LUDO_DEBUG
		ID3D12Debug* m_D3D12Debug = nullptr;
		IDXGIDebug1* m_DXGIDebug = nullptr;
#endif 
	};

}