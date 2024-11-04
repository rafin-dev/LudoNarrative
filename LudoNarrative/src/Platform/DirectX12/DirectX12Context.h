#pragma once

#include "ldpch.h"
#include "Ludo/Renderer/GraphicsContext.h"

namespace Ludo {

	class WindowsWindow;

	class DirectX12Context : public GraphicsContext
	{
	public:
		DirectX12Context(HWND window);
		~DirectX12Context() override;

		bool Init() override;
		void Resize(unsigned int width, unsigned int height) override;

		void SwapBuffers() override;

		static constexpr size_t SwapChainBufferCount = 2;
		static constexpr size_t GetSwapChainBufferCount()
		{
			return SwapChainBufferCount;
		}

	private:
		void ShutDown();
		void ResizeImpl();

		inline void BeginFrame();
		inline void EndFrame();

		inline bool RetrieveBuffers();
		inline void ReleaseBuffers();

		IDXGISwapChain4* m_SwapChain = nullptr;
		ID3D12Resource2* m_Buffers[SwapChainBufferCount] = {};
		size_t m_CurrentBackBuffer = 0;

		ID3D12DescriptorHeap* m_rtvDescriptorHeap = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE m_rtvCPUhandles[SwapChainBufferCount] = {};

		ID3D12Resource2* m_UploadBuffer = nullptr;
		ID3D12Resource2* m_VertexBuffer = nullptr;

		ID3D12RootSignature* m_RootSignature = nullptr;
		ID3D12PipelineState* m_PipelineStateObject = nullptr;

		bool m_ShouldResize = false;
		unsigned int m_Nwidth = 0;
		unsigned int m_Nheight = 0;
		HWND m_WindowHandle = nullptr;
		WindowsWindow* m_Window = nullptr;
	};

}