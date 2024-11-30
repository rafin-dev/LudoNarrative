#pragma once

#include "ldpch.h"
#include "Ludo/Core.h"

#include "Ludo/Renderer/Buffer.h"

namespace Ludo {

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			DirectX12 = 1,
			DirectX11 = 2
		};

	public:
		virtual void ShutDown() = 0;

		virtual bool Init() = 0;

		virtual void SetClearColor(const DirectX::XMFLOAT4& color) = 0;
		virtual const DirectX::XMFLOAT4& GetClearColor() = 0;

		virtual void DrawIndexed(const std::shared_ptr<VertexBuffer>& vertexBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

		virtual void BeginImGui() = 0;
		virtual void EndImGui() = 0;

		static inline API GetAPI() { return s_API; }

	private:
		static API s_API;
	};

}