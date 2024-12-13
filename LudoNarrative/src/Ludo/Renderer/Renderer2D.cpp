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
		Ref<Material> TextureMaterial;
	};

	static DataRenderer2D* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new DataRenderer2D();

		// ========== Vertex Array ==========
		float vertices[] =
		{
			-0.50f, -0.50f, 0.0f, 0.0f, 0.0f,
			-0.50f,  0.50f, 0.0f, 0.0f, 1.0f,
			 0.50f,  0.50f, 0.0f, 1.0f, 1.0f,
			 0.50f, -0.50f, 0.0f, 1.0f, 0.0f
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
		auto vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices), { { "Position", Ludo::ShaderDataType::Float3 }, { "TexCoord", ShaderDataType::Float2 } });
		auto indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(indices[0]));
		s_Data->QuadVertexArray = VertexArray::Create();
		s_Data->QuadVertexArray->AddVertexBuffer(vertexBuffer);
		s_Data->QuadVertexArray->SetIndexBuffer(indexBuffer);

		BufferLayout materialLayout = {
			{ "Color", ShaderDataType::Float4 },
		};
		auto flatColorShader = Shader::Create("FlatColorShader", "assets/shaders/FlatColorShader.hlsl", vertexBuffer->GetLayout(), materialLayout);
		s_Data->FlatColorMaterial = Material::Create(flatColorShader);

		auto textureShader = Shader::Create("TextureShader", "assets/shaders/TextureShader.hlsl", vertexBuffer->GetLayout(), {});
		s_Data->TextureMaterial = Material::Create(textureShader);
	}
	
	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}

	static DirectX::XMFLOAT4X4 cam;
	
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_Data->FlatColorMaterial->GetShader()->SetViewProjectionMatrix(camera.GetViewProjectionMatrix());
		s_Data->TextureMaterial->GetShader()->SetViewProjectionMatrix(camera.GetViewProjectionMatrix());

		cam = camera.GetViewProjectionMatrix();
	}
	
	void Renderer2D::EndScene()
	{
	}
	
	void Renderer2D::DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, float rotation, const DirectX::XMFLOAT4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, const DirectX::XMFLOAT4& color)
	{
		s_Data->FlatColorMaterial->GetShader()->Bind();

		DirectX::XMFLOAT4X4 transform;		
		DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixTranspose(
			DirectX::XMMatrixScaling(size.x, size.y, 1.0f) *
			DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(-rotation)) *
			DirectX::XMMatrixTranslation(position.x, position.y, position.z)));

		s_Data->FlatColorMaterial->GetShader()->SetModelMatrix(transform);

		s_Data->FlatColorMaterial->SetMaterialItemData("Color", &color);
		s_Data->FlatColorMaterial->UploadMaterialData();

		DirectX::XMVECTOR pos = DirectX::XMVector3Transform(
			DirectX::XMVector3Transform(
				DirectX::XMLoadFloat3(&position),
				DirectX::XMLoadFloat4x4(&cam)
			), DirectX::XMMatrixScaling(size.x, size.y, 1.0f) *
			DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(-rotation)) *
			DirectX::XMMatrixTranslation(position.x, position.y, position.z));
		DirectX::XMFLOAT3 poss;
		DirectX::XMStoreFloat3(&poss, pos);

		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<Texture2D>& texture)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, rotation, texture);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<Texture2D>& texture)
	{
		s_Data->TextureMaterial->GetShader()->Bind();

		DirectX::XMFLOAT4X4 transform;
		DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixTranspose(
			DirectX::XMMatrixScaling(size.x, size.y, 1.0f) *
			DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(-rotation)) *
			DirectX::XMMatrixTranslation(position.x, position.y, position.z)));

		s_Data->TextureMaterial->GetShader()->SetModelMatrix(transform);

		s_Data->TextureMaterial->UploadMaterialData();
		
		texture->Bind();
		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

}