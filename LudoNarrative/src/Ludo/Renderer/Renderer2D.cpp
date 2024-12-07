#include "ldpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Material.h"
#include "RenderCommand.h"

namespace Ludo {

	struct DataRenderer2D
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Material> FlatColorMaterial;
	};

	static DataRenderer2D* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new DataRenderer2D();

		// ========== Vertex Array ==========
		float vertices[] =
		{
			-0.50f, -0.50f, 0.0f,
			-0.50f,  0.50f, 0.0f,
			 0.50f,  0.50f, 0.0f,
			 0.50f, -0.50f, 0.0f
		};
		uint32_t indices[] =
		{
			0,
			1,
			2,
			2,
			3,
			0
		};
		auto vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices), { { "Position", Ludo::ShaderDataType::Float3 } });
		auto indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(indices[0]));
		s_Data->QuadVertexArray = VertexArray::Create();
		s_Data->QuadVertexArray->AddVertexBuffer(vertexBuffer);
		s_Data->QuadVertexArray->SetIndexBuffer(indexBuffer);

		BufferLayout materialLayout = {
			{ "Color", ShaderDataType::Float4 }
		};
		auto shader = Shader::Create("FlatColorShader", "assets/shaders/FlatColorShader.hlsl", vertexBuffer->GetLayout(), materialLayout);
		s_Data->FlatColorMaterial = Material::Create(shader);
	}
	
	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}
	
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_Data->FlatColorMaterial->GetShader()->SetViewProjectionMatrix(camera.GetViewProjectionMatrix());
	}
	
	void Renderer2D::EndScene()
	{
	}
	
	void Renderer2D::DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT4& color)
	{
		s_Data->FlatColorMaterial->SetMaterialItemData("Color", &color);
		DirectX::XMFLOAT4X4 transform;
		DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());
		s_Data->FlatColorMaterial->GetShader()->SetModelMatrix(transform);

		s_Data->FlatColorMaterial->UploadMaterialData();
		s_Data->FlatColorMaterial->GetShader()->Bind();
		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

}