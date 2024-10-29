#pragma once

#include "ldpch.h"
#include "Ludo/Renderer/GraphicsContext.h"

namespace Ludo {

	class DirectX12Context : public GraphicsContext
	{
	public:
		DirectX12Context(HWND window);
		~DirectX12Context() override;

		bool Init() override;
		void Resize(unsigned int width, unsigned int height) override;

		void SetFullScreen(bool enabled);

		void SwapBuffers() override;

		static constexpr size_t GetSwapChainBufferCount()
		{
			return 2;
		}

	private:
		void ShutDown();

		IDXGISwapChain4* m_SwapChain = nullptr;

		HWND m_Window = nullptr;
	};

}