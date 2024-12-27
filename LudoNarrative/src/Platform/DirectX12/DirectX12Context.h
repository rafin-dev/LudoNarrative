#pragma once

#include "Ludo/Renderer/GraphicsContext.h"
#include "Platform/Windows/WindowsWindow.h"

#include <d3d12.h>

namespace Ludo {

	class DirectX12Context : public GraphicsContext
	{
	public:
		DirectX12Context(HWND window);
		virtual bool Init() override;
		virtual ~DirectX12Context() override;
		virtual void SwapBuffers() override;

		virtual void Resize(uint32_t width, uint32_t height) override;

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

		ID3D12Resource2* m_DepthStencilBuffer = nullptr;
		ID3D12DescriptorHeap* m_DepthStencilDescriptorHeap = nullptr;

		bool m_ShouldResize = false;
		uint32_t m_Nwidth = 0;
		uint32_t m_Nheight = 0;
		HWND m_WindowHandle = nullptr;
		WindowsWindow* m_Window = nullptr;
	};

}