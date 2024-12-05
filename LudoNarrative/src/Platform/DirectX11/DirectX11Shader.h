#pragma once

#include "Ludo/Renderer/Shader.h"

namespace Ludo {

	class DirectX11Shader : public Shader
	{
	public:
		enum ShaderKind
		{
			VertexShader = 0,
			PixelShader
		};

		DirectX11Shader(
			const std::string& name,
			void* vertexShaderBuffer, size_t vertexShaderSize,
			void* pixelShaderBuffer, size_t pixelShaderSize,
			const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout);
		DirectX11Shader(
			const std::string& name,
			const std::filesystem::path& shaderSrcPath,
			const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout
		);

		bool Init(std::unordered_map<ShaderKind, ID3DBlob*> shaders);

		virtual ~DirectX11Shader() override;

		virtual void Bind() override;

		virtual const std::string& GetName() const override { return m_Name; }

		virtual void SetViewProjectionMatrix(const DirectX::XMFLOAT4X4& matrix) override;
		virtual void SetModelMatrix(const DirectX::XMFLOAT4X4& matrix) override;

		virtual void UploadMaterialData(void* data) override;

		virtual const BufferLayout& GetVertexBufferLayout() override;
		virtual const BufferLayout& GetMaterialLayout() override;

	private:
		void ShutDown();
		void ReadFile(const std::filesystem::path& file, std::string& output);
		void ParseShaders(const std::string& source, std::unordered_map<ShaderKind, std::string>& shadersSrcs);
		void CompileShaders(const std::unordered_map<ShaderKind, std::string> shadersSources, std::unordered_map<ShaderKind, ID3D10Blob*>& output);

		std::string m_Name;

		ID3D11VertexShader* m_VertexShader = nullptr;
		ID3D11PixelShader* m_PixelShader = nullptr;
		ID3D11InputLayout* m_InputLayout = nullptr;

		ID3D11Buffer* m_ViewProjectionBuffer = nullptr;
		ID3D11Buffer* m_ModelMatrixBuffer = nullptr;
		ID3D11Buffer* m_MaterialBuffer = nullptr;

		BufferLayout m_VertexBufferLayout;
		BufferLayout m_MaterialDataLayout;
	};

}