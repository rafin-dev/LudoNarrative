#include "Ludo/Renderer/GraphicsContext.h"

namespace Ludo {

	class DirectX11Context : public GraphicsContext
	{
	public:
		DirectX11Context(HWND window);
		bool Init() override;
		~DirectX11Context() override;

		void SwapBuffers() override;
		
		void Clear() override;

		void Resize(unsigned int width, unsigned int height);

	private:
		void ShutDown();

		void CreateRenderTarget();

		HWND m_Window;

		IDXGISwapChain1* m_SwapChain = nullptr;
		ID3D11RenderTargetView* m_BackBuffer = nullptr;
	};
}