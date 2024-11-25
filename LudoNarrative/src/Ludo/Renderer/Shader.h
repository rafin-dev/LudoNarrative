#pragma once

#include "ldpch.h"
#include "Buffer.h"

namespace Ludo {

	class MaterialDataLayout;

	struct LUDO_SHADER_DESC
	{
		// Vertex shader
		void* VertexShaderBlob = nullptr;
		size_t VertexShaderSize = 0;
		
		// Pixel Shader
		void* PixelShaderBlob = nullptr;
		size_t PixelShaderSize = 0;

		// Vertex Buffer Layout
		BufferLayout VertexBufferLayout = {};

		// Material Data Layout
		BufferLayout MaterialDataLayout = {};
	};

	class Shader 
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() = 0;

		virtual void SetViewProjectionMatrix(const DirectX::XMFLOAT4X4& matrix) = 0;
		virtual void SetModelMatrix(const DirectX::XMFLOAT4X4& matrix) = 0;

		virtual void AddEntry() = 0;
		virtual void RemoveEntry() = 0;

		virtual void UploadMaterialDataBuffer(void* data) = 0;

		virtual void NextEntry() = 0;

		virtual const BufferLayout& GetVertexBufferLayout() = 0;
		virtual const BufferLayout& GetMaterialLayout() = 0;

		static Shader* Create(const LUDO_SHADER_DESC& desc);
	};

}