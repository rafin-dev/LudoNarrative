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

	static Renderer2DData s_ScriptingData;

	void Renderer2D::Init()
	{
		LD_PROFILE_FUNCTION();

		uint32_t* Indices = new uint32_t[s_ScriptingData.MaxQuadIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_ScriptingData.MaxQuadIndices; i += 6)
		{
			Indices[i + 0] = offset + 0;
			Indices[i + 1] = offset + 1;
			Indices[i + 2] = offset + 2;

			Indices[i + 3] = offset + 2;
			Indices[i + 4] = offset + 3;
			Indices[i + 5] = offset + 0;

			offset += 4;
		}

		s_ScriptingData.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);

		auto indexBuffer = IndexBuffer::Create(Indices, s_ScriptingData.MaxQuadIndices);
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

			s_ScriptingData.QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxQuadVertices * sizeof(QuadVertex), quadVbLayout, VertexBuffer::DYNAMIC);
			s_ScriptingData.QuadVertexArray = VertexArray::Create();
			s_ScriptingData.QuadVertexArray->AddVertexBuffer(s_ScriptingData.QuadVertexBuffer);
			s_ScriptingData.QuadVertexArray->SetIndexBuffer(indexBuffer);

			s_ScriptingData.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MaxQuadVertices];

			s_ScriptingData.QuadShader = Shader::Create("TextureShader", "assets/shaders/Renderer2D_QuadShader.hlsl", quadVbLayout, {});

			s_ScriptingData.WhiteTexture = Texture2D::Create(1, 1);
			uint32_t whiteTextureData = 0xffffffff;
			s_ScriptingData.WhiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));

			s_ScriptingData.TextureSlots[0] = s_ScriptingData.WhiteTexture;

			s_ScriptingData.VertexPositions[0] = { -0.50f, -0.50f, 0.0f };
			s_ScriptingData.VertexPositions[1] = { -0.50f,  0.50f, 0.0f };
			s_ScriptingData.VertexPositions[2] = { 0.50f,  0.50f, 0.0f };
			s_ScriptingData.VertexPositions[3] = { 0.50f, -0.50f, 0.0f };
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

			s_ScriptingData.CircleVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxCircleVertices * sizeof(CircleVertex), circleVbLayout, VertexBuffer::DYNAMIC);
			s_ScriptingData.CircleVertexArray = VertexArray::Create();
			s_ScriptingData.CircleVertexArray->AddVertexBuffer(s_ScriptingData.CircleVertexBuffer);
			s_ScriptingData.CircleVertexArray->SetIndexBuffer(indexBuffer);

			s_ScriptingData.CircleShader = Shader::Create("CircleShader", "assets/shaders/Renderer2D_CircleShader.hlsl", circleVbLayout, {});

			s_ScriptingData.CircleVertexBufferBase = new CircleVertex[Renderer2DData::MaxCircleVertices];
		}

		// Lines
		{
			BufferLayout lineVbLayout = {
				{ "Position",	ShaderDataType::Float3	},
				{ "Color",		ShaderDataType::Float4	},
				{ "EntityID",	ShaderDataType::Int		}
			};

			s_ScriptingData.LineVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxLinesVertices * sizeof(CircleVertex), lineVbLayout, VertexBuffer::DYNAMIC);
			s_ScriptingData.LineVertexArray = VertexArray::Create();
			s_ScriptingData.LineVertexArray->AddVertexBuffer(s_ScriptingData.LineVertexBuffer);
			s_ScriptingData.LineVertexArray->SetIndexBuffer(indexBuffer);

			s_ScriptingData.LineShader = Shader::Create("LineShader", "assets/shaders/Renderer2D_LineShader.hlsl", lineVbLayout, {});

			s_ScriptingData.LineVertexBufferBase = new LineVertex[Renderer2DData::MaxLinesVertices];
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
			delete[] s_ScriptingData.QuadVertexBufferBase;
			for (auto& texture : s_ScriptingData.TextureSlots) { texture = nullptr; }
			s_ScriptingData.QuadVertexArray.reset();
			s_ScriptingData.QuadVertexBuffer.reset();
			s_ScriptingData.QuadShader.reset();
			s_ScriptingData.WhiteTexture.reset();
		}

		// Circles
		{
			delete[] s_ScriptingData.CircleVertexBufferBase;
			s_ScriptingData.CircleVertexArray.reset();
			s_ScriptingData.CircleVertexBuffer.reset();
			s_ScriptingData.CircleShader.reset();
		}

		// Lines
		{
			delete[] s_ScriptingData.LineVertexBufferBase;
			s_ScriptingData.LineVertexArray.reset();
			s_ScriptingData.LineVertexBuffer.reset();
			s_ScriptingData.LineShader.reset();
		}

		s_ScriptingData.CameraUniformBuffer.reset();
	}
	
	void Renderer2D::BeginScene(const Camera& camera, const DirectX::XMFLOAT4X4& transform)
	{
		LD_PROFILE_RENDERER_FUNCTION();

		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, DirectX::XMMatrixTranspose(
			DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&transform)) * DirectX::XMLoadFloat4x4(&camera.GetProjection())));

		s_ScriptingData.CameraBuffer.ViewProjectionMatrix = viewProjection;
		s_ScriptingData.CameraUniformBuffer->SetData(&s_ScriptingData.CameraBuffer, sizeof(Renderer2DData::CameraData), 0);

		ClearData();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		DirectX::XMFLOAT4X4 viewProjection = camera.GetViewProjection();
		DirectX::XMStoreFloat4x4(&viewProjection, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&viewProjection)));
		s_ScriptingData.CameraBuffer.ViewProjectionMatrix = viewProjection;
		s_ScriptingData.CameraUniformBuffer->SetData(&s_ScriptingData.CameraBuffer, sizeof(Renderer2DData::CameraData), 0);

		ClearData();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		LD_PROFILE_RENDERER_FUNCTION();

		s_ScriptingData.CameraBuffer.ViewProjectionMatrix = camera.GetViewProjectionMatrix();
		s_ScriptingData.CameraUniformBuffer->SetData(&s_ScriptingData.CameraBuffer, sizeof(Renderer2DData::CameraData), 0);

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

		if (s_ScriptingData.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_ScriptingData.QuadVertexBufferPtr - (uint8_t*)s_ScriptingData.QuadVertexBufferBase);
			s_ScriptingData.QuadVertexBuffer->SetData(s_ScriptingData.QuadVertexBufferBase, dataSize);

			for (uint32_t i = 0; i < s_ScriptingData.TextureSlotIndex; i++)
			{
				s_ScriptingData.TextureSlots[i]->Bind(i);
			}

			s_ScriptingData.QuadShader->Bind();
			s_ScriptingData.CameraUniformBuffer->Bind();
			RenderCommand::DrawIndexed(s_ScriptingData.QuadVertexArray, s_ScriptingData.QuadIndexCount);
		
			s_ScriptingData.Stats.TotalDrawCalls++;
			s_ScriptingData.Stats.QuadDrawCalls++;
		}

		if (s_ScriptingData.CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_ScriptingData.CircleVertexBufferPtr - (uint8_t*)s_ScriptingData.CircleVertexBufferBase);
			s_ScriptingData.CircleVertexBuffer->SetData(s_ScriptingData.CircleVertexBufferBase, dataSize);

			s_ScriptingData.CircleShader->Bind();
			s_ScriptingData.CameraUniformBuffer->Bind();
			RenderCommand::DrawIndexed(s_ScriptingData.CircleVertexArray, s_ScriptingData.CircleIndexCount);

			s_ScriptingData.Stats.TotalDrawCalls++;
			s_ScriptingData.Stats.CircleDrawCalls++;
		}

		if (s_ScriptingData.LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_ScriptingData.LineVertexBufferPtr - (uint8_t*)s_ScriptingData.LineVertexBufferBase);
			s_ScriptingData.LineVertexBuffer->SetData(s_ScriptingData.LineVertexBufferBase, dataSize);

			s_ScriptingData.LineShader->Bind();
			s_ScriptingData.CameraUniformBuffer->Bind();
			RenderCommand::DrawLines(s_ScriptingData.LineVertexArray, s_ScriptingData.LineVertexCount);

			s_ScriptingData.Stats.TotalDrawCalls++;
			s_ScriptingData.Stats.LineDrawCalls++;
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

		if (s_ScriptingData.LineVertexCount == Renderer2DData::MaxLinesVertices)
		{
			FlushAndReset();
		}

		s_ScriptingData.LineVertexBufferPtr->Position = begin;
		s_ScriptingData.LineVertexBufferPtr->Color = color;
		s_ScriptingData.LineVertexBufferPtr->EntityID = entityID;
		s_ScriptingData.LineVertexBufferPtr++;

		s_ScriptingData.LineVertexBufferPtr->Position = end;
		s_ScriptingData.LineVertexBufferPtr->Color = color;
		s_ScriptingData.LineVertexBufferPtr->EntityID = entityID;
		s_ScriptingData.LineVertexBufferPtr++;

		s_ScriptingData.LineVertexCount += 2;

		s_ScriptingData.Stats.TotalObjectCount++;
		s_ScriptingData.Stats.LineCount++;
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
			DirectX::XMStoreFloat3(points + i, DirectX::XMVector2Transform(DirectX::XMLoadFloat3(s_ScriptingData.VertexPositions + i), transform));
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

		if (s_ScriptingData.QuadIndexCount == Renderer2DData::MaxQuadIndices)
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
			DirectX::XMStoreFloat3(&s_ScriptingData.QuadVertexBufferPtr->Position,
				DirectX::XMVector2Transform(DirectX::XMLoadFloat3(&s_ScriptingData.VertexPositions[i]), transform));
			s_ScriptingData.QuadVertexBufferPtr->Color = color;
			s_ScriptingData.QuadVertexBufferPtr->TexCoord = texCoords[i];
			s_ScriptingData.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
			s_ScriptingData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_ScriptingData.QuadVertexBufferPtr->EntityID = entityID;
			s_ScriptingData.QuadVertexBufferPtr++;
		}
		s_ScriptingData.QuadIndexCount += 6;

		s_ScriptingData.Stats.TotalObjectCount++;
		s_ScriptingData.Stats.QuadCount++;
	}

	void LD_SIMD_CALLING_CONVENTION Renderer2D::DrawQuad(int entityID, const DirectX::XMMATRIX& transform, const Ref<Texture2D>& texture, const DirectX::XMFLOAT2* texCoords, const DirectX::XMFLOAT4& tintColor, float tilingFactor)
	{
		LD_PROFILE_RENDERER_FUNCTION();

		if (s_ScriptingData.QuadIndexCount == Renderer2DData::MaxQuadIndices)
		{
			FlushAndReset();
		}

		uint32_t textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_ScriptingData.TextureSlotIndex; i++)
		{
			if (s_ScriptingData.TextureSlots[i] == texture)
			{
				textureIndex = i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = s_ScriptingData.TextureSlotIndex;
			s_ScriptingData.TextureSlots[s_ScriptingData.TextureSlotIndex] = texture;
			s_ScriptingData.TextureSlotIndex++;
		}

		for (int i = 0; i < 4; i++)
		{
			DirectX::XMStoreFloat3(&s_ScriptingData.QuadVertexBufferPtr->Position, DirectX::XMVector2Transform(DirectX::XMLoadFloat3(&s_ScriptingData.VertexPositions[i]), transform));
			s_ScriptingData.QuadVertexBufferPtr->Color = tintColor;
			s_ScriptingData.QuadVertexBufferPtr->TexCoord = texCoords[i];
			s_ScriptingData.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_ScriptingData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_ScriptingData.QuadVertexBufferPtr->EntityID = entityID;
			s_ScriptingData.QuadVertexBufferPtr++;
		}

		s_ScriptingData.QuadIndexCount += 6;

		s_ScriptingData.Stats.TotalObjectCount++;
		s_ScriptingData.Stats.QuadCount++;
	}

	void LD_SIMD_CALLING_CONVENTION Renderer2D::DrawCircle(int EntityID, const DirectX::XMMATRIX& transform, const DirectX::XMFLOAT4& color, float thickness, float fade)
	{
		LD_PROFILE_FUNCTION();

		if (s_ScriptingData.CircleIndexCount == Renderer2DData::MaxCircleIndices)
		{
			FlushAndReset();
		}


		for (uint32_t i = 0; i < 4; i++)
		{
			auto vertexPosition = DirectX::XMLoadFloat3(&s_ScriptingData.VertexPositions[i]);

			DirectX::XMStoreFloat3(&s_ScriptingData.CircleVertexBufferPtr->WorldPosition,
				DirectX::XMVector2Transform(vertexPosition, transform));

			DirectX::XMStoreFloat3(&s_ScriptingData.CircleVertexBufferPtr->LocalPosition,
				DirectX::XMVectorScale(vertexPosition, 2.0f));
			
			s_ScriptingData.CircleVertexBufferPtr->Color = color;
			s_ScriptingData.CircleVertexBufferPtr->Thickness = thickness;
			s_ScriptingData.CircleVertexBufferPtr->Fade = fade;
			s_ScriptingData.CircleVertexBufferPtr->EntityID = EntityID;
			s_ScriptingData.CircleVertexBufferPtr++;
		}
		s_ScriptingData.CircleIndexCount += 6;

		s_ScriptingData.Stats.TotalObjectCount++;
		s_ScriptingData.Stats.CircleCount++;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_ScriptingData.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_ScriptingData.Stats;
	}

	void Renderer2D::ClearData()
	{
		s_ScriptingData.QuadIndexCount = 0;
		s_ScriptingData.QuadVertexBufferPtr = s_ScriptingData.QuadVertexBufferBase;
		s_ScriptingData.TextureSlotIndex = 1;

		s_ScriptingData.CircleIndexCount = 0;
		s_ScriptingData.CircleVertexBufferPtr = s_ScriptingData.CircleVertexBufferBase;

		s_ScriptingData.LineVertexCount = 0;
		s_ScriptingData.LineVertexBufferPtr = s_ScriptingData.LineVertexBufferBase;
	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();

		s_ScriptingData.QuadIndexCount = 0;
		s_ScriptingData.QuadVertexBufferPtr = s_ScriptingData.QuadVertexBufferBase;
		s_ScriptingData.TextureSlotIndex = 1;
	}

}