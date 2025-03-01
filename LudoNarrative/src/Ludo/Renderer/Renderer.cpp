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

	void Renderer::Submit(const Ref<Material>& material, const Ref<VertexArray>& vertexArray, const DirectX::XMFLOAT4X4& transform)
	{
		auto& shader = material->GetShader();
		shader->Bind();
		material->UploadMaterialData();
		
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}