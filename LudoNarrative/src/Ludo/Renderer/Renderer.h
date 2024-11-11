#pragma once

#include "RenderCommand.h"
#include "Shader.h"

#include "OrthographicCamera.h"

namespace Ludo {

	class Renderer
	{
	public:
		static inline bool Init() { return RenderCommand::Init(); }

		static void BeginScene(OrthographicCamera& camera); // TODO: Get scene info
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexBuffer>& vertexBuffer, const std::shared_ptr<IndexBuffer> indexBuffer);

		static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			DirectX::XMFLOAT4X4 ViewProjectionMatrix;
		};

		static SceneData* s_SceneData;
	};

}