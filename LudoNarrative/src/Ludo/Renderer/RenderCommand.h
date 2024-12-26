#pragma once

#include "ldpch.h"
#include "RendererAPI.h"

namespace Ludo {

	class RenderCommand
	{
	public:
		static inline bool Init()
		{
			bool success = s_RenderAPI->Init();
			return success;
		}

		static inline void ShutDown()
		{
			s_RenderAPI->ShutDown();
		}

		static inline void SetClearColor(const DirectX::XMFLOAT4& color)
		{
			s_RenderAPI->SetClearColor(color);
		}

		static inline const DirectX::XMFLOAT4& GetClearColor()
		{
			return s_RenderAPI->GetClearColor();
		}

		static inline void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RenderAPI->DrawIndexed(vertexArray);
		}

		static void BeginImGui()
		{
			s_RenderAPI->BeginImGui();
		}

		static void EndImGui()
		{
			s_RenderAPI->EndImGui();
		}

	private:
		static RendererAPI* s_RenderAPI;
	};

}