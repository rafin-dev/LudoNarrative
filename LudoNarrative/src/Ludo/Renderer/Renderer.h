#pragma once

#include "RenderCommand.h"
#include "Shader.h"

#include "OrthographicCamera.h"

namespace Ludo {

	class Renderer
	{
	public:
		static inline bool Init() { return RenderCommand::Init(); }
		static inline void ShutDown() { RenderCommand::ShutDown(); }

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, 
			const Ref<VertexBuffer>& vertexBuffer, 
			const Ref<IndexBuffer>& indexBuffer, 
			const DirectX::XMFLOAT4X4& transform);

		static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			DirectX::XMFLOAT4X4 ViewProjectionMatrix;
		};

		static SceneData* s_SceneData;
	};

}