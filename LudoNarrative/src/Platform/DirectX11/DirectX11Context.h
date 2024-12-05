#pragma once

#include "ldpch.h"
#include "Ludo/Renderer/GraphicsContext.h"
#include "Platform/Windows/WindowsWindow.h"

namespace Ludo {

	class DirectX11Context : public GraphicsContext
	{
	public:
		DirectX11Context(HWND window);
		virtual ~DirectX11Context() override;

		virtual bool Init() override;
		virtual void SwapBuffers() override;

		virtual void Resize(unsigned int width, unsigned int height) override;

		static constexpr size_t SwapChainBufferCount = 2;
		static constexpr size_t GetSwapChainBufferCount()
		{
			return SwapChainBufferCount;
		}


	private:
		void ShutDown();

		bool GetBackBuffer();
		void ResizeImpl();

		void SetViewPort();

		IDXGISwapChain2* m_SwapChain = nullptr;

		ID3D11RenderTargetView* m_BackBuffer = nullptr;

		HWND m_WindowHandle = NULL;
		WindowsWindow* m_Window = nullptr;
		bool m_ShouldResize = false;
	};
}