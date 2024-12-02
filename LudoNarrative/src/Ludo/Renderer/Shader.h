#pragma once

#include "Buffer.h"

#include <string>
#include <unordered_map>
#include <filesystem>

namespace Ludo {

	class Shader 
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() = 0;

		virtual const std::string& GetName() const = 0;

		virtual void SetViewProjectionMatrix(const DirectX::XMFLOAT4X4& matrix) = 0;
		virtual void SetModelMatrix(const DirectX::XMFLOAT4X4& matrix) = 0;

		virtual void UploadMaterialData(void* data) = 0;

		virtual const BufferLayout& GetVertexBufferLayout() = 0;
		virtual const BufferLayout& GetMaterialLayout() = 0;

		static Ref<Shader> Create(
								const std::string& name,
								void* vertexShaderBuffer, size_t vertexShaderSize,
								void* pixelShaderBuffer, size_t pixelShaderSize,
								const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout);

		static Ref<Shader> Create(const std::string& name, const std::filesystem::path& vertexSrcPath, const std::filesystem::path& pixelSrcPath,
			const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout);
	};

	class ShaderLibrary
	{
	public:
		void Add(const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& name, const std::filesystem::path& vertexSrc, const std::filesystem::path& pixelSrc,
			const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout);

		Ref<Shader> Load(
			const std::string& name, 
			void* vertexShaderBuffer, size_t vertexShaderSize,
			void* pixelShaderBuffer, size_t pixelShaderSize,
			const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout);

		Ref<Shader> Get(const std::string& name);

		bool Exists(const std::string& name) const;

	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;

	};

}