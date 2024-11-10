#pragma once

namespace Ludo {

	enum class RendererAPI
	{
		None = 0,
		DirectX12 = 1
	};

	class Renderer
	{
	public:
		static inline RendererAPI GetAPI() { return s_RenderAPI; }

	private:
		static RendererAPI s_RenderAPI;
	};

}