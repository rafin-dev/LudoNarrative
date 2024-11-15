#pragma once

#include "ldpch.h"
#include "Ludo/Renderer/Shader.h"

namespace Ludo {

	class DirectX12Shader : public Shader
	{
	public:
		DirectX12Shader(const LUDO_SHADER_DESC& desc);
		bool Init(const LUDO_SHADER_DESC& desc);
		virtual ~DirectX12Shader() override;

		virtual void Bind() override;

		virtual void SetViewProjectionMatrix(const DirectX::XMFLOAT4X4& matrix) override;
		virtual void SetModelMatrix(const DirectX::XMFLOAT4X4& matrix) override;

		static bool InitSystem();
		static void CloseSystem();

	private:
		void ShutDown();

		ID3D12PipelineState* m_PipelineStateObject = nullptr;
		ID3D12RootSignature* m_RootSignature = nullptr;

		static ID3D12RootSignature* m_2DRootSignature;
		static std::array<D3D12_INPUT_ELEMENT_DESC, 1> m_2DElementLayout;
	};

}