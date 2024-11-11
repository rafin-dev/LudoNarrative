#include "ldpch.h"
#include "Renderer.h"

#include "RenderCommand.h"

namespace Ludo {

	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexBuffer>& vertexBuffer, const std::shared_ptr<IndexBuffer> indexBuffer)
	{
		shader->Bind();
		shader->UploadUniformMat4(s_SceneData->ViewProjectionMatrix);

		vertexBuffer->Bind();
		indexBuffer->Bind();
		RenderCommand::DrawIndexed(vertexBuffer, indexBuffer);
	}

}