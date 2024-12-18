#include "ldpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Material.h"
#include "RenderCommand.h"

namespace Ludo {

	static void CreateTransform(DirectX::XMFLOAT4X4* matrixOutput, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT2& size, float rotation)
	{
		DirectX::XMStoreFloat4x4(matrixOutput, DirectX::XMMatrixTranspose(
			DirectX::XMMatrixScaling(size.x, size.y, 1.0f) *
			DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(-rotation)) *
			DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z)));
	}

	struct DataRenderer2D
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Material> TextureMaterial;
		Ref<Texture2D> WhiteTexture;
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

		auto textureShader = Shader::Create("TextureShader", "assets/shaders/TextureShader.hlsl", vertexBuffer->GetLayout(), materialLayout);
		s_Data->TextureMaterial = Material::Create(textureShader);

		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));
	}
	
	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}
	
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_Data->TextureMaterial->GetShader()->Bind();
		s_Data->TextureMaterial->GetShader()->SetViewProjectionMatrix(camera.GetViewProjectionMatrix());
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
		DirectX::XMFLOAT4X4 transform;		
		CreateTransform(&transform, position, size, rotation);

		s_Data->TextureMaterial->GetShader()->SetModelMatrix(transform);

		s_Data->TextureMaterial->SetMaterialItemData("Color", &color);
		s_Data->TextureMaterial->UploadMaterialData();

		s_Data->WhiteTexture->Bind();
		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<Texture2D>& texture, const DirectX::XMFLOAT4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, rotation, texture, color);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<Texture2D>& texture, const DirectX::XMFLOAT4& color)
	{
		DirectX::XMFLOAT4X4 transform;
		CreateTransform(&transform, position, size, rotation);

		s_Data->TextureMaterial->GetShader()->SetModelMatrix(transform);
		s_Data->TextureMaterial->SetMaterialItemData("Color", &color);
		s_Data->TextureMaterial->UploadMaterialData();
		
		texture->Bind();
		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

}