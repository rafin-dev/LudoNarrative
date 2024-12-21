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

	struct QuadVertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT4 Color;
		DirectX::XMFLOAT2 TexCoord;
	};

	struct Renderer2DData
	{
		const uint32_t MaxQuads = 10000;
		const uint32_t MaxVertices = MaxQuads * 4;
		const uint32_t MaxIndices = MaxQuads * 6;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Material> TextureMaterial;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;
	};

	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		LD_PROFILE_FUNCTION();

		// ========== Vertex Array ==========
		BufferLayout vbLayout = {
			{ "Position", ShaderDataType::Float3 },
			{ "Color", ShaderDataType::Float4 },
			{ "TexCoord", ShaderDataType::Float2 }
		};

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex), vbLayout);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];
		
		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		auto quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		delete[] quadIndices;

		s_Data.QuadVertexArray = VertexArray::Create();
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);

		BufferLayout materialLayout = {
			{ "Color", ShaderDataType::Float4 },
			{ "TilingFactor", ShaderDataType::Float }
		};

		auto textureShader = Shader::Create("TextureShader", "assets/shaders/TextureShader.hlsl", s_Data.QuadVertexBuffer->GetLayout(), materialLayout);
		s_Data.TextureMaterial = Material::Create(textureShader);

		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));
	}
	
	void Renderer2D::Shutdown()
	{
		LD_PROFILE_FUNCTION();

		delete[] s_Data.QuadVertexBufferBase;
	}
	
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		LD_PROFILE_FUNCTION();

		s_Data.TextureMaterial->GetShader()->Bind();
		s_Data.TextureMaterial->GetShader()->SetViewProjectionMatrix(camera.GetViewProjectionMatrix());

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
	}
	
	void Renderer2D::EndScene()
	{
		LD_PROFILE_FUNCTION();


		Flush();
	}

	void Renderer2D::Flush()
	{
		LD_PROFILE_FUNCTION();

		uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
	}
	
	void Renderer2D::DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, float rotation, const DirectX::XMFLOAT4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, const DirectX::XMFLOAT4& color)
	{
		LD_PROFILE_FUNCTION();

		float halfWidth = size.x / 2;
		float halfHeight = size.y / 2;

		s_Data.QuadVertexBufferPtr->Position = { position.x - halfWidth, position.y - halfHeight, position.z };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = { position.x - halfWidth, position.y + halfHeight, position.z };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = { position.x + halfWidth, position.y + halfHeight, position.z };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = { position.x + halfWidth, position.y - halfHeight, position.z };
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 6;

		DirectX::XMFLOAT4X4 transform;		
		CreateTransform(&transform, position, size, rotation);

		/*s_Data.TextureMaterial->GetShader()->SetModelMatrix(transform);

		float tilingFactor = 1.0f;
		s_Data.TextureMaterial->SetMaterialItemData("TilingFactor", &tilingFactor);
		s_Data.TextureMaterial->UploadMaterialData();

		s_Data.WhiteTexture->Bind();
		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);*/
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<Texture2D>& texture, const DirectX::XMFLOAT4& color, float tilingFactor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, rotation, texture, color, tilingFactor);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<Texture2D>& texture, const DirectX::XMFLOAT4& color, float tilingFactor)
	{
		LD_PROFILE_FUNCTION();

		DirectX::XMFLOAT4X4 transform;
		CreateTransform(&transform, position, size, rotation);

		s_Data.TextureMaterial->GetShader()->SetModelMatrix(transform);
		s_Data.TextureMaterial->SetMaterialItemData("Color", &color);
		s_Data.TextureMaterial->SetMaterialItemData("TilingFactor", &tilingFactor);
		s_Data.TextureMaterial->UploadMaterialData();
		
		texture->Bind();
		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
	}

}