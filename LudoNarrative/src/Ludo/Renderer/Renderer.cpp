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

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer> indexBuffer, const DirectX::XMFLOAT4X4& transform)
	{
		shader->Bind();
		shader->SetViewProjectionMatrix(s_SceneData->ViewProjectionMatrix);
		shader->SetModelMatrix(transform);

		vertexBuffer->Bind();
		indexBuffer->Bind();
		RenderCommand::DrawIndexed(vertexBuffer, indexBuffer);
	}

}