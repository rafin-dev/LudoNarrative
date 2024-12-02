#pragma once

#include "Ludo/Renderer/Shader.h"

namespace Ludo {

	class DirectX11Shader : public Shader
	{
	public:
		DirectX11Shader(
			const std::string& name,
			void* vertexShaderBuffer, size_t vertexShaderSize,
			void* pixelShaderBuffer, size_t pixelShaderSize,
			const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout);
		DirectX11Shader(
			const std::string& name,
			const std::filesystem::path& vertexSrcPath, const std::filesystem::path& pixelSrcPath,
			const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout
		);

		bool Init(
			void* vertexShaderBuffer, size_t vertexShaderSize,
			void* pixelShaderBuffer, size_t pixelShaderSize);

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