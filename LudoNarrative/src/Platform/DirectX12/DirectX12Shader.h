#pragma once

#include "Ludo/Renderer/Shader.h"

#include <d3d12.h>
#include <DirectXMath.h>

#include "Platform/DirectX12/Utils/DX12UploadBuffer.h"
#include "Platform/DirectX12/Utils/DX12ShaderCompiler.h"

namespace Ludo {

	class DirectX12Shader : public Shader
	{
	public:

		DirectX12Shader(const std::string& name,
			void* vertexShaderBuffer, size_t vertexShaderSize,
			void* pixelShaderBuffer, size_t pixelShaderSize,
			const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout);
		DirectX12Shader(const std::string& name, const std::filesystem::path& shaderSrcPath, const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout);

		virtual ~DirectX12Shader() override;

		virtual void Bind() override;

		virtual const std::string& GetName() const override;

		virtual void SetViewProjectionMatrix(const DirectX::XMFLOAT4X4& matrix) override;
		virtual void SetModelMatrix(const DirectX::XMFLOAT4X4& matrix) override;

		virtual void UploadMaterialData(void* data) override;

		virtual const BufferLayout& GetVertexBufferLayout() override;
		virtual const BufferLayout& GetMaterialLayout() override;
		
		static bool CreateRootSignature();
		static void DestroyRootSignature();

	private:
		bool Init(std::unordered_map<DX12ShaderCompiler::ShaderKind, IDxcBlob*> shaders);
		void ShutDown();

		ID3D12PipelineState* CreateTrianglePSOforFrameBufferFormats(std::vector<DXGI_FORMAT>& formats);
		ID3D12PipelineState* CreateLinePSOforFrameBufferFormats(std::vector<DXGI_FORMAT>& formats);

		void ReadFile(const std::filesystem::path& file, std::string& output);
		void ParseShaders(const std::string& source, std::unordered_map<DX12ShaderCompiler::ShaderKind, std::string>& shadersSrcs);
		void CompileShaders(const std::unordered_map<DX12ShaderCompiler::ShaderKind, std::string> shadersSources, 
							std::unordered_map<DX12ShaderCompiler::ShaderKind, IDxcBlob*>& output);

		std::unordered_map<DX12ShaderCompiler::ShaderKind, IDxcBlob*> m_Shaders;
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_ElementLayout;

		std::map<std::vector<DXGI_FORMAT>, ID3D12PipelineState*> m_TrianglePipelineStates;
		std::map<std::vector<DXGI_FORMAT>, ID3D12PipelineState*> m_LinePipelineStates;

		// To be used by the DirectX12 API
		bool m_LineBound = false;
		
		std::string m_Name = "None";

		std::vector<DX12UploadBuffer> m_ShaderResources;

		uint32_t m_LastFrame = 0;
		uint32_t m_CurrentShaderResource = 0;

		BufferLayout m_VertexBufferLayout;
		BufferLayout m_MaterialLayout;

		static ID3D12RootSignature* s_RootSignature;

		static IDxcUtils* s_DxCompilerUtils;
		static IDxcCompiler3* s_DxCompiler;

		static DirectX12Shader* s_CurrentShader;

		friend class DirectX12API;
	};

}