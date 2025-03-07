#pragma once

#include "ldpch.h"
#include "Ludo/Core/Core.h"

#include "Ludo/Renderer/VertexArray.h"

namespace Ludo {

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			DirectX11 = 1
		};

	public:
		virtual bool Init() = 0;
		virtual void ShutDown() = 0;

		virtual void SetClearColor(const DirectX::XMFLOAT4& color) = 0;
		virtual const DirectX::XMFLOAT4& GetClearColor() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount = 0) = 0;

		virtual void BeginImGui() = 0;
		virtual void EndImGui() = 0;

		static inline API GetAPI() { return s_API; }

	private:
		static API s_API;
	};

}