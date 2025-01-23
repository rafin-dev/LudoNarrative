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

	// Operator to make Texture2D comparison synatx look a bit nicer
	inline bool operator==(const Ref<Texture2D>& a, const Ref<Texture2D>& b)
	{
		return *a.get() == *b.get();
	}

	struct QuadVertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT4 Color;
		DirectX::XMFLOAT2 TexCoord;
		float TexIndex;
		float TilingFactor;
		int EntityID;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxtextureSlots = 32;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Material> TextureMaterial;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxtextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;

		DirectX::XMFLOAT3 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;
	};

	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		LD_PROFILE_FUNCTION();

		// ========== Vertex Array ==========
		BufferLayout vbLayout = {
			{ "Position", ShaderDataType::Float3 },
			{ "Color", ShaderDataType::Float4 },
			{ "TexCoord", ShaderDataType::Float2 },
			{ "TexIndex", ShaderDataType::Float },
			{ "TilingFactor", ShaderDataType::Float },
			{ "EntityID", ShaderDataType::Int }
		};

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex), vbLayout, VertexBuffer::DYNAMIC);

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

		auto textureShader = Shader::Create("TextureShader", "assets/shaders/TextureShader.hlsl", s_Data.QuadVertexBuffer->GetLayout(), {});
		s_Data.TextureMaterial = Material::Create(textureShader);

		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPositions[0] = { -0.50f, -0.50f, 0.0f };
		s_Data.QuadVertexPositions[1] = { -0.50f,  0.50f, 0.0f };
		s_Data.QuadVertexPositions[2] = {  0.50f,  0.50f, 0.0f };
		s_Data.QuadVertexPositions[3] = {  0.50f, -0.50f, 0.0f };
	}
	
	void Renderer2D::Shutdown()
	{
		LD_PROFILE_RENDERER_FUNCTION();

		delete[] s_Data.QuadVertexBufferBase;
		for (auto& texture : s_Data.TextureSlots) { texture = nullptr; }
		s_Data.QuadVertexArray = nullptr;
		s_Data.QuadVertexBuffer = nullptr;
		s_Data.TextureMaterial = nullptr;
		s_Data.WhiteTexture = nullptr;
	}
	
	void Renderer2D::BeginScene(const Camera& camera, const DirectX::XMFLOAT4X4& transform)
	{
		LD_PROFILE_RENDERER_FUNCTION();

		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, DirectX::XMMatrixTranspose(
			DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&transform)) * DirectX::XMLoadFloat4x4(&camera.GetProjection())));

		s_Data.TextureMaterial->GetShader()->Bind();
		s_Data.TextureMaterial->GetShader()->SetViewProjectionMatrix(viewProjection);

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		s_Data.TextureMaterial->GetShader()->Bind();

		DirectX::XMFLOAT4X4 viewProjection = camera.GetViewProjection();
		DirectX::XMStoreFloat4x4(&viewProjection, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&viewProjection)));
		s_Data.TextureMaterial->GetShader()->SetViewProjectionMatrix(viewProjection);

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		LD_PROFILE_RENDERER_FUNCTION();

		s_Data.TextureMaterial->GetShader()->Bind();
		s_Data.TextureMaterial->GetShader()->SetViewProjectionMatrix(camera.GetViewProjectionMatrix());

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;
	}
	
	void Renderer2D::EndScene()
	{
		LD_PROFILE_RENDERER_FUNCTION();

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		Flush();
	}

	void Renderer2D::Flush()
	{
		LD_PROFILE_RENDERER_FUNCTION();

		if (s_Data.QuadIndexCount == 0)
		{
			return;
		}

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
		{
			s_Data.TextureSlots[i]->Bind(i);
		}

		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);

		s_Data.Stats.DrawCalls++;
	}
	
	
	void Renderer2D::DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, float rotation, const DirectX::XMFLOAT4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, const DirectX::XMFLOAT4& color)
	{
		DirectX::XMMATRIX transform = DirectX::XMMatrixScaling(size.x, size.y, 1.0f) *
			DirectX::XMMatrixRotationZ(-rotation) *
			DirectX::XMMatrixTranslation(position.x, position.y, position.z);

		DrawQuad(-1, transform, color);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT4X4& transform, const DirectX::XMFLOAT4& color)
	{
		DrawQuad(-1, DirectX::XMLoadFloat4x4(&transform), color);
	}

	void LD_SIMD_CALLING_CONVENTION Renderer2D::DrawQuad(const DirectX::XMMATRIX& transform, const DirectX::XMFLOAT4& color)
	{
		DrawQuad(-1, transform, color);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<Texture2D>& texture, const DirectX::XMFLOAT4& color, float tilingFactor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, rotation, texture, color, tilingFactor);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<Texture2D>& texture, const DirectX::XMFLOAT4& color, float tilingFactor)
	{
		DirectX::XMFLOAT2 texCoords[] = {
			{ 0.0f, 0.0f },
			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },
			{ 1.0f, 0.0f }
		};

		DrawQuad(position, size, rotation, texture, texCoords, color, tilingFactor);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT4X4& transform, const Ref<Texture2D>& texture, const DirectX::XMFLOAT4& color, float tilingFactor)
	{
		DirectX::XMFLOAT2 texCoords[] = {
			{ 0.0f, 0.0f },
			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },
			{ 1.0f, 0.0f }
		};

		DrawQuad(-1, DirectX::XMLoadFloat4x4(&transform), texture, texCoords, color, tilingFactor);
	}

	void LD_SIMD_CALLING_CONVENTION Renderer2D::DrawQuad(const DirectX::XMMATRIX& transform, const Ref<Texture2D>& texture, const DirectX::XMFLOAT4& color, float tilingFactor)
	{
		DirectX::XMFLOAT2 texCoords[] = {
			{ 0.0f, 0.0f },
			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },
			{ 1.0f, 0.0f }
		};

		DrawQuad(-1, transform, texture, texCoords, color, tilingFactor);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<SubTexture2D>& subTexture, const DirectX::XMFLOAT4& color, float tilingFactor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, rotation, subTexture, color, tilingFactor);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<SubTexture2D>& subTexture, const DirectX::XMFLOAT4& color, float tilingFactor)
	{
		DirectX::XMMATRIX transform = DirectX::XMMatrixScaling(size.x, size.y, 1.0f) *
			DirectX::XMMatrixRotationZ(-rotation) *
			DirectX::XMMatrixTranslation(position.x, position.y, position.z);

		DrawQuad(-1, transform, subTexture->GetTexture(), subTexture->GetTexCoords(), color, tilingFactor);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, float rotation, const Ref<Texture2D>& texture, const DirectX::XMFLOAT2* texCoords, const DirectX::XMFLOAT4& color, float tilingFactor)
	{
		DirectX::XMMATRIX transform = DirectX::XMMatrixScaling(size.x, size.y, 1.0f) *
			DirectX::XMMatrixRotationZ(-rotation) *
			DirectX::XMMatrixTranslation(position.x, position.y, position.z);

		DrawQuad(-1, transform, texture, texCoords, color, tilingFactor);
	}

	void LD_SIMD_CALLING_CONVENTION Renderer2D::DrawQuad(const DirectX::XMMATRIX& transform, const Ref<SubTexture2D>& subTexture, const DirectX::XMFLOAT4& color, float tilingFactor)
	{
		DrawQuad(-1, transform, subTexture->GetTexture(), subTexture->GetTexCoords(), color, tilingFactor);
	}

	void LD_SIMD_CALLING_CONVENTION Renderer2D::DrawQuad(int entityID, const DirectX::XMMATRIX& transform, const Ref<SubTexture2D>& subTexture, const DirectX::XMFLOAT4& color, float tilingFactor)
	{
		DrawQuad(entityID, transform, subTexture->GetTexture(), subTexture->GetTexCoords(), color, tilingFactor);
	}

	void LD_SIMD_CALLING_CONVENTION Renderer2D::DrawQuad(int entityID, const DirectX::XMMATRIX& transform, const DirectX::XMFLOAT4& color)
	{
		LD_PROFILE_RENDERER_FUNCTION();

		if (s_Data.QuadIndexCount == Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		constexpr float whiteTextureIndex = 0.0f;
		constexpr float tilingFactor = 1.0f;

		DirectX::XMFLOAT2 texCoords[] = {
			{ 0.0f, 0.0f },
			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },
			{ 1.0f, 0.0f }
		};

		for (int i = 0; i < 4; i++)
		{
			DirectX::XMStoreFloat3(&s_Data.QuadVertexBufferPtr->Position,
				DirectX::XMVector2Transform(DirectX::XMLoadFloat3(&s_Data.QuadVertexPositions[i]), transform));
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = texCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void LD_SIMD_CALLING_CONVENTION Renderer2D::DrawQuad(int entityID, const DirectX::XMMATRIX& transform, const Ref<Texture2D>& texture, const DirectX::XMFLOAT2* texCoords, const DirectX::XMFLOAT4& tintColor, float tilingFactor)
	{
		LD_PROFILE_RENDERER_FUNCTION();

		if (s_Data.QuadIndexCount == Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (s_Data.TextureSlots[i] == texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (int i = 0; i < 4; i++)
		{
			DirectX::XMStoreFloat3(&s_Data.QuadVertexBufferPtr->Position, DirectX::XMVector2Transform(DirectX::XMLoadFloat3(&s_Data.QuadVertexPositions[i]), transform));
			s_Data.QuadVertexBufferPtr->Color = tintColor;
			s_Data.QuadVertexBufferPtr->TexCoord = texCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;
	}

}