#pragma once

#include "Ludo/Renderer/Shader.h"

namespace Ludo {

	class DirectX11Shader : public Shader
	{
	public:
		DirectX11Shader(const LUDO_SHADER_DESC& desc);
		bool Init(const LUDO_SHADER_DESC& desc);
		virtual ~DirectX11Shader() override;

		virtual void Bind() override;

		virtual void SetViewProjectionMatrix(const DirectX::XMFLOAT4X4& matrix) override;
		virtual void SetModelMatrix(const DirectX::XMFLOAT4X4& matrix) override;

		virtual void UploadMaterialData(void* data) override;

		virtual const BufferLayout& GetVertexBufferLayout() override;
		virtual const BufferLayout& GetMaterialLayout() override;

	private:
		void ShutDown();

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