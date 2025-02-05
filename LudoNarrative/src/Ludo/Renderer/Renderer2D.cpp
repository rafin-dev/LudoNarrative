#include "ldpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Material.h"
#include "RenderCommand.h"
#include "UniformBuffer.h"

namespace Ludo {

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
		uint32_t TexIndex;
		float TilingFactor;

		// Editor only
		int EntityID;
	};

	struct CircleVertex
	{
		DirectX::XMFLOAT3 WorldPosition;
		DirectX::XMFLOAT3 LocalPosition;
		DirectX::XMFLOAT4 Color;
		float Thickness;
		float Fade;

		// Editor only
		int EntityID;
	};

	struct LineVertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT4 Color;

		// Editor only
		int EntityID;
	};

	struct Renderer2DData
	{
		// Quads
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxQuadVertices = MaxQuads * 4;
		static const uint32_t MaxQuadIndices = MaxQuads * 6;
		static const uint32_t MaxtextureSlots = 32;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxtextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;

		// Circles
		static const uint32_t MaxCircles = 20000;
		static const uint32_t MaxCircleVertices = MaxCircles * 4;
		static const uint32_t MaxCircleIndices = MaxCircles * 6;

		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		// Lines
		static const uint32_t MaxLines = 20000;
		static const uint32_t MaxLinesVertices = MaxCircles * 4;

		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		///////////////////////////////////////////////
		DirectX::XMFLOAT3 VertexPositions[4];

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			DirectX::XMFLOAT4X4 ViewProjectionMatrix;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		LD_PROFILE_FUNCTION();

		uint32_t* Indices = new uint32_t[s_Data.MaxQuadIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxQuadIndices; i += 6)
		{
			Indices[i + 0] = offset + 0;
			Indices[i + 1] = offset + 1;
			Indices[i + 2] = offset + 2;

			Indices[i + 3] = offset + 2;
			Indices[i + 4] = offset + 3;
			Indices[i + 5] = offset + 0;

			offset += 4;
		}

		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);

		auto indexBuffer = IndexBuffer::Create(Indices, s_Data.MaxQuadIndices);
		delete[] Indices;

		// Quads
		{
			// ========== Vertex Array ==========
			BufferLayout quadVbLayout = {
				{ "Position",		ShaderDataType::Float3	},
				{ "Color",			ShaderDataType::Float4	},
				{ "TexCoord",		ShaderDataType::Float2	},
				{ "TexIndex",		ShaderDataType::Uint	},
				{ "TilingFactor",	ShaderDataType::Float	},
				{ "EntityID",		ShaderDataType::Int		}
			};

			s_Data.QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxQuadVertices * sizeof(QuadVertex), quadVbLayout, VertexBuffer::DYNAMIC);
			s_Data.QuadVertexArray = VertexArray::Create();
			s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);
			s_Data.QuadVertexArray->SetIndexBuffer(indexBuffer);

			s_Data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MaxQuadVertices];

			s_Data.QuadShader = Shader::Create("TextureShader", "assets/shaders/Renderer2D_QuadShader.hlsl", quadVbLayout, {});

			s_Data.WhiteTexture = Texture2D::Create(1, 1);
			uint32_t whiteTextureData = 0xffffffff;
			s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));

			s_Data.TextureSlots[0] = s_Data.WhiteTexture;

			s_Data.VertexPositions[0] = { -0.50f, -0.50f, 0.0f };
			s_Data.VertexPositions[1] = { -0.50f,  0.50f, 0.0f };
			s_Data.VertexPositions[2] = { 0.50f,  0.50f, 0.0f };
			s_Data.VertexPositions[3] = { 0.50f, -0.50f, 0.0f };
		}
		
		// Circles
		{
			BufferLayout circleVbLayout = {
				{ "WorldPos",	ShaderDataType::Float3	},
				{ "LocalPos",	ShaderDataType::Float3	},
				{ "Color",		ShaderDataType::Float4	},
				{ "Thickness",	ShaderDataType::Float	},
				{ "Fade",		ShaderDataType::Float	},
				{ "EntityID",	ShaderDataType::Int		}
			};

			s_Data.CircleVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxCircleVertices * sizeof(CircleVertex), circleVbLayout, VertexBuffer::DYNAMIC);
			s_Data.CircleVertexArray = VertexArray::Create();
			s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
			s_Data.CircleVertexArray->SetIndexBuffer(indexBuffer);

			s_Data.CircleShader = Shader::Create("CircleShader", "assets/shaders/Renderer2D_CircleShader.hlsl", circleVbLayout, {});

			s_Data.CircleVertexBufferBase = new CircleVertex[Renderer2DData::MaxCircleVertices];
		}

		// Lines
		{
			BufferLayout lineVbLayout = {
				{ "Position",	ShaderDataType::Float3	},
				{ "Color",		ShaderDataType::Float4	},
				{ "EntityID",	ShaderDataType::Int		}
			};

			s_Data.LineVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxLinesVertices * sizeof(CircleVertex), lineVbLayout, VertexBuffer::DYNAMIC);
			s_Data.LineVertexArray = VertexArray::Create();
			s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
			s_Data.LineVertexArray->SetIndexBuffer(indexBuffer);

			s_Data.LineShader = Shader::Create("LineShader", "assets/shaders/Renderer2D_LineShader.hlsl", lineVbLayout, {});

			s_Data.LineVertexBufferBase = new LineVertex[Renderer2DData::MaxLinesVertices];
		}
	}
	
	void Renderer2D::Shutdown()
	{
		LD_PROFILE_RENDERER_FUNCTION();

		// Explicitly remove references to any Renderer Object
		// Otherwise it will only get deleted after the Renderer has shutdown
		// Leading to issues with D3D12 COM reference counting
		
		// Quads
		{
			delete[] s_Data.QuadVertexBufferBase;
			for (auto& texture : s_Data.TextureSlots) { texture = nullptr; }
			s_Data.QuadVertexArray.reset();
			s_Data.QuadVertexBuffer.reset();
			s_Data.QuadShader.reset();
			s_Data.WhiteTexture.reset();
		}

		// Circles
		{
			delete[] s_Data.CircleVertexBufferBase;
			s_Data.CircleVertexArray.reset();
			s_Data.CircleVertexBuffer.reset();
			s_Data.CircleShader.reset();
		}

		// Lines
		{
			delete[] s_Data.LineVertexBufferBase;
			s_Data.LineVertexArray.reset();
			s_Data.LineVertexBuffer.reset();
			s_Data.LineShader.reset();
		}

		s_Data.CameraUniformBuffer.reset();
	}
	
	void Renderer2D::BeginScene(const Camera& camera, const DirectX::XMFLOAT4X4& transform)
	{
		LD_PROFILE_RENDERER_FUNCTION();

		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, DirectX::XMMatrixTranspose(
			DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&transform)) * DirectX::XMLoadFloat4x4(&camera.GetProjection())));

		s_Data.CameraBuffer.ViewProjectionMatrix = viewProjection;
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData), 0);

		ClearData();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		DirectX::XMFLOAT4X4 viewProjection = camera.GetViewProjection();
		DirectX::XMStoreFloat4x4(&viewProjection, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&viewProjection)));
		s_Data.CameraBuffer.ViewProjectionMatrix = viewProjection;
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData), 0);

		ClearData();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		LD_PROFILE_RENDERER_FUNCTION();

		s_Data.CameraBuffer.ViewProjectionMatrix = camera.GetViewProjectionMatrix();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData), 0);

		ClearData();
	}
	
	void Renderer2D::EndScene()
	{
		LD_PROFILE_RENDERER_FUNCTION();

		Flush();
	}

	void Renderer2D::Flush()
	{
		LD_PROFILE_RENDERER_FUNCTION();

		if (s_Data.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			{
				s_Data.TextureSlots[i]->Bind(i);
			}

			s_Data.QuadShader->Bind();
			s_Data.CameraUniformBuffer->Bind();
			RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
		
			s_Data.Stats.TotalDrawCalls++;
			s_Data.Stats.QuadDrawCalls++;
		}

		if (s_Data.CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase);
			s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);

			s_Data.CircleShader->Bind();
			s_Data.CameraUniformBuffer->Bind();
			RenderCommand::DrawIndexed(s_Data.CircleVertexArray, s_Data.CircleIndexCount);

			s_Data.Stats.TotalDrawCalls++;
			s_Data.Stats.CircleDrawCalls++;
		}

		if (s_Data.LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
			s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

			s_Data.LineShader->Bind();
			s_Data.CameraUniformBuffer->Bind();
			RenderCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineVertexCount);

			s_Data.Stats.TotalDrawCalls++;
			s_Data.Stats.LineDrawCalls++;
		}
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

	void Renderer2D::DrawCircle(const DirectX::XMFLOAT4X4& transform, const DirectX::XMFLOAT4& color, float thickness, float fade)
	{
		DrawCircle(-1, DirectX::XMLoadFloat4x4(&transform), color, thickness, fade);
	}

	void LD_SIMD_CALLING_CONVENTION Renderer2D::DrawCircle(const DirectX::XMMATRIX& transform, const DirectX::XMFLOAT4& color, float thickness, float fade)
	{
		DrawCircle(-1, transform, color, thickness, fade);
	}

	void LD_SIMD_CALLING_CONVENTION Renderer2D::DrawSprite(const DirectX::XMMATRIX& transform, const SpriteRendererComponent& sprite, int entityID)
	{
		if (sprite.Texture)
		{
			DrawQuad(entityID, transform, sprite.Texture->GetTexture(), sprite.Texture->GetTexCoords(), sprite.Color, sprite.TilingFactor);
		}
		else
		{
			DrawQuad(entityID, transform, sprite.Color);
		}
	}

	void LD_SIMD_CALLING_CONVENTION Renderer2D::DrawCircle(const DirectX::XMMATRIX& transform, const CircleRendererComponent& circle, int entityID)
	{
		DrawCircle(entityID, transform, circle.Color, circle.Thickness, circle.Fade);
	}

	void Renderer2D::DrawLine(const DirectX::XMFLOAT3& begin, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT4& color, int entityID)
	{
		LD_PROFILE_RENDERER_FUNCTION();

		if (s_Data.LineVertexCount == Renderer2DData::MaxLinesVertices)
		{
			FlushAndReset();
		}

		s_Data.LineVertexBufferPtr->Position = begin;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexBufferPtr->Position = end;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexCount += 2;

		s_Data.Stats.TotalObjectCount++;
		s_Data.Stats.LineCount++;
	}

	void Renderer2D::DrawRect(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT4& color, int entityID)
	{
		DirectX::XMMATRIX transform =
			DirectX::XMMatrixScaling(size.x, size.y, 1.0f) *
			DirectX::XMMatrixTranslation(position.x, position.y, position.z);

		DrawRect(transform, color, entityID);
	}

	void Renderer2D::DrawRect(const DirectX::XMFLOAT4X4& transform, const DirectX::XMFLOAT4& color, int entityID)
	{
		DrawRect(DirectX::XMLoadFloat4x4(&transform), color, entityID);
	}

	void Renderer2D::DrawRect(const DirectX::XMMATRIX& transform, const DirectX::XMFLOAT4& color, int entityID)
	{
		DirectX::XMFLOAT3 points[4];

		for (uint32_t i = 0; i < 4; i++)
		{
			DirectX::XMStoreFloat3(points + i, DirectX::XMVector2Transform(DirectX::XMLoadFloat3(s_Data.VertexPositions + i), transform));
		}

		for (uint32_t i = 0; i < 4; i++)
		{
			DrawLine(points[i], points[(i + 1) % 4], color, entityID);
		}
	}

	void LD_SIMD_CALLING_CONVENTION Renderer2D::DrawQuad(int entityID, const DirectX::XMMATRIX& transform, const Ref<SubTexture2D>& subTexture, const DirectX::XMFLOAT4& color, float tilingFactor)
	{
		DrawQuad(entityID, transform, subTexture->GetTexture(), subTexture->GetTexCoords(), color, tilingFactor);
	}

	void LD_SIMD_CALLING_CONVENTION Renderer2D::DrawQuad(int entityID, const DirectX::XMMATRIX& transform, const DirectX::XMFLOAT4& color)
	{
		LD_PROFILE_RENDERER_FUNCTION();

		if (s_Data.QuadIndexCount == Renderer2DData::MaxQuadIndices)
		{
			FlushAndReset();
		}

		constexpr uint32_t whiteTextureIndex = 0;
		constexpr float tilingFactor = 1.0f;

		DirectX::XMFLOAT2 texCoords[] = {
			{ 0.0f, 0.0f },
			{ 0.0f, 1.0f },
			{ 1.0f, 1.0f },
			{ 1.0f, 0.0f }
		};

		for (uint32_t i = 0; i < 4; i++)
		{
			DirectX::XMStoreFloat3(&s_Data.QuadVertexBufferPtr->Position,
				DirectX::XMVector2Transform(DirectX::XMLoadFloat3(&s_Data.VertexPositions[i]), transform));
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = texCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 6;

		s_Data.Stats.TotalObjectCount++;
		s_Data.Stats.QuadCount++;
	}

	void LD_SIMD_CALLING_CONVENTION Renderer2D::DrawQuad(int entityID, const DirectX::XMMATRIX& transform, const Ref<Texture2D>& texture, const DirectX::XMFLOAT2* texCoords, const DirectX::XMFLOAT4& tintColor, float tilingFactor)
	{
		LD_PROFILE_RENDERER_FUNCTION();

		if (s_Data.QuadIndexCount == Renderer2DData::MaxQuadIndices)
		{
			FlushAndReset();
		}

		uint32_t textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (s_Data.TextureSlots[i] == texture)
			{
				textureIndex = i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (int i = 0; i < 4; i++)
		{
			DirectX::XMStoreFloat3(&s_Data.QuadVertexBufferPtr->Position, DirectX::XMVector2Transform(DirectX::XMLoadFloat3(&s_Data.VertexPositions[i]), transform));
			s_Data.QuadVertexBufferPtr->Color = tintColor;
			s_Data.QuadVertexBufferPtr->TexCoord = texCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.TotalObjectCount++;
		s_Data.Stats.QuadCount++;
	}

	void LD_SIMD_CALLING_CONVENTION Renderer2D::DrawCircle(int EntityID, const DirectX::XMMATRIX& transform, const DirectX::XMFLOAT4& color, float thickness, float fade)
	{
		LD_PROFILE_FUNCTION();

		if (s_Data.CircleIndexCount == Renderer2DData::MaxCircleIndices)
		{
			FlushAndReset();
		}


		for (uint32_t i = 0; i < 4; i++)
		{
			auto vertexPosition = DirectX::XMLoadFloat3(&s_Data.VertexPositions[i]);

			DirectX::XMStoreFloat3(&s_Data.CircleVertexBufferPtr->WorldPosition,
				DirectX::XMVector2Transform(vertexPosition, transform));

			DirectX::XMStoreFloat3(&s_Data.CircleVertexBufferPtr->LocalPosition,
				DirectX::XMVectorScale(vertexPosition, 2.0f));
			
			s_Data.CircleVertexBufferPtr->Color = color;
			s_Data.CircleVertexBufferPtr->Thickness = thickness;
			s_Data.CircleVertexBufferPtr->Fade = fade;
			s_Data.CircleVertexBufferPtr->EntityID = EntityID;
			s_Data.CircleVertexBufferPtr++;
		}
		s_Data.CircleIndexCount += 6;

		s_Data.Stats.TotalObjectCount++;
		s_Data.Stats.CircleCount++;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

	void Renderer2D::ClearData()
	{
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;

		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

		s_Data.LineVertexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;
	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;
	}

}