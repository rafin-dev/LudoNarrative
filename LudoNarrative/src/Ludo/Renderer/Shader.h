#pragma once

#include "ldpch.h"
#include "Buffer.h"

namespace Ludo {

	enum LUDO_SHADER_TARGET_PIPELINE
	{
		LUDO_TARGET_PIPELINE_2D = 0,
		LUDO_TARGET_PIPELINE_3D = 1
	};

	struct LUDO_SHADER_DESC
	{
		// Vertex shader
		void* VertexShaderBlob = nullptr;
		size_t VertexShaderSize = 0;
		
		// Pixel Shader
		void* PixelShaderBlob = nullptr;
		size_t PixelShaderSize = 0;

		// Target Pipeline
		LUDO_SHADER_TARGET_PIPELINE TargetPipeline = LUDO_TARGET_PIPELINE_2D;

		// Vertex Buffer Layout
		BufferLayout Layout = {};
	};

	class Shader 
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() = 0;

		virtual void SetViewProjectionMatrix(const DirectX::XMFLOAT4X4& matrix) = 0;
		virtual void SetModelMatrix(const DirectX::XMFLOAT4X4& matrix) = 0;

		static Shader* Create(const LUDO_SHADER_DESC& desc);
	};

}